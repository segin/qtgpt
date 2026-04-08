#include "QtGPT.h"
#include "PipeHandler.h"
#include "PluginManager.h"
#include "utils.h"
#include "SettingsDialog.h"
#include <QCoreApplication>
#include <QStandardPaths>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

QtGPT *QtGPT::s_instance = nullptr;

QtGPT::QtGPT(QObject *parent)
    : QObject(parent)
{
    s_instance = this;
    m_dpContext = nullptr;
    m_pluginManager = nullptr;
    m_pipeHandler = nullptr;
    m_mainWindow = nullptr;
    m_settingsDialog = nullptr;

    // Initialize with defaults
    m_provider = DP_PROVIDER_GOOGLE_GEMINI;
    m_maxHistoryMessages = 100;
    m_appendSystemPrompt = true;
    m_enterSendsMessage = true;

    // Load config paths
    m_pluginDirectory = Utils::getUserConfigDir() + "/plugins";

    // Initialize pipe handler
    m_pipeHandler = new PipeHandler(this);

    // Initialize plugin manager
    m_pluginManager = new PluginManager(this);
    m_pluginManager->setPluginDirectory(m_pluginDirectory);
}

QtGPT::~QtGPT()
{
    destroyContext();
}

QtGPT *QtGPT::instance()
{
    return s_instance;
}

void QtGPT::initContext()
{
    m_mutex.lock();
    
    QString api_key;
    switch (m_provider) {
        case DP_PROVIDER_GOOGLE_GEMINI:
            api_key = m_geminiApiKey;
            break;
        case DP_PROVIDER_OPENAI_COMPATIBLE:
            api_key = m_openaiApiKey;
            break;
        case DP_PROVIDER_ANTHROPIC:
            api_key = m_anthropicApiKey;
            break;
    }

    QString base_url;
    if (m_provider == DP_PROVIDER_OPENAI_COMPATIBLE) {
        base_url = m_openaiBaseUrl;
    }

    m_dpContext = dp_init_context_with_app_info(m_provider, api_key.toUtf8().data(), 
                                                 base_url.isEmpty() ? nullptr : base_url.toUtf8().data(),
                                                 "QtGPT", "1.0.0");

    m_mutex.unlock();

    if (m_dpContext) {
        m_pluginManager->loadPlugins(m_pluginDirectory);
        m_pluginManager->registerBuiltInTools();
    }
}

void QtGPT::destroyContext()
{
    m_mutex.lock();
    if (m_dpContext) {
        dp_destroy_context(m_dpContext);
        m_dpContext = nullptr;
    }
    m_pluginManager->unloadPlugins();
    m_mutex.unlock();
}

bool QtGPT::loadSettings()
{
    QString configPath = QStandardPaths::locate(QStandardPaths::ConfigLocation, "opencode/qtgpt/settings.conf");
    
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (configPath.isEmpty()) {
        configPath = appDataPath + "/settings.conf";
    }

    QFile configFile(configPath);
    if (!configFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QTextStream in(&configFile);

    QString section, line;
    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) continue;
        
        if (line.startsWith('[') && line.endsWith(']')) {
            section = line.mid(1, line.length() - 2);
            continue;
        }
        
        int eqIdx = line.indexOf('=');
        if (eqIdx > 0 && section.isEmpty()) continue;
        
        QString key = line.left(eqIdx).trimmed();
        QString value = line.mid(eqIdx + 1).trimmed();
        
        if (section == "General") {
            if (key == "Provider") {
                if (value == "gemini") setProvider(DP_PROVIDER_GOOGLE_GEMINI);
                else if (value == "openai") setProvider(DP_PROVIDER_OPENAI_COMPATIBLE);
                else if (value == "anthropic") setProvider(DP_PROVIDER_ANTHROPIC);
            } else if (key == "HistoryLimit") {
                setMaxHistoryMessages(value.toInt());
            } else if (key == "SystemPrompt") {
                QString decoded = value;
                decoded = decoded.replace("\\n", "\n");
                setSystemPrompt(decoded);
            } else if (key == "AppendSystemPrompt") {
                setAppendSystemPrompt(value == "true");
            } else if (key == "EnterSendsMessage") {
                setEnterSendsMessage(value == "true");
            }
        } else if (section == "Gemini") {
            if (key == "ApiKey") setGeminiApiKey(value);
            else if (key == "ModelId") setGeminiModel(value);
        } else if (section == "OpenAI") {
            if (key == "ApiKey") setOpenaiApiKey(value);
            else if (key == "ModelId") setOpenaiModel(value);
            else if (key == "BaseUrl") setOpenaiBaseUrl(value);
        } else if (section == "Anthropic") {
            if (key == "ApiKey") setAnthropicApiKey(value);
            else if (key == "ModelId") setAnthropicModel(value);
        } else if (section == "Plugins") {
            if (key == "Directory") setPluginDirectory(value);
        }
    }

    configFile.close();
    
    QString currentProviderStr = "";
    switch (m_provider) {
        case DP_PROVIDER_GOOGLE_GEMINI: currentProviderStr = "gemini"; break;
        case DP_PROVIDER_OPENAI_COMPATIBLE: currentProviderStr = "openai"; break;
        case DP_PROVIDER_ANTHROPIC: currentProviderStr = "anthropic"; break;
    }
    
    if (m_settingsDialog && currentProviderStr == m_settingsDialog->m_currentProvider) {
        m_settingsDialog->populateSettings();
    }
    
    return true;
}

bool QtGPT::saveSettings()
{
    QString configPath = QStandardPaths::locate(QStandardPaths::ConfigLocation, "opencode/qtgpt/settings.conf");
    
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (configPath.isEmpty()) {
        configPath = appDataPath + "/settings.conf";
    }
    
    QFile configFile(configPath);
    if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QString parentPath = QFileInfo(configFile).path();
        QDir().mkpath(parentPath);
        if (!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            return false;
        }
    }

    QTextStream out(&configFile);

    out << "# QtGPT Settings\n\n";

    QString providerStr;
    switch (m_provider) {
        case DP_PROVIDER_GOOGLE_GEMINI: providerStr = "gemini"; break;
        case DP_PROVIDER_OPENAI_COMPATIBLE: providerStr = "openai"; break;
        case DP_PROVIDER_ANTHROPIC: providerStr = "anthropic"; break;
    }

    out << "[General]\n";
    out << "Provider=" << providerStr << "\n";
    out << "HistoryLimit=" << m_maxHistoryMessages << "\n";
    out << "SaveHistory=true\n";
    out << "LoadHistory=true\n";
    out << "SystemPrompt=" << m_systemPrompt.replace("\n", "\\n").replace("\r", "\\n") << "\n\n";

    out << "[Gemini]\n";
    out << "ApiKey=" << m_geminiApiKey << "\n";
    out << "ModelId=" << m_geminiModel << "\n\n";

    out << "[OpenAI]\n";
    out << "ApiKey=" << m_openaiApiKey << "\n";
    out << "ModelId=" << m_openaiModel << "\n\n";

    out << "[Anthropic]\n";
    out << "ApiKey=" << m_anthropicApiKey << "\n";
    out << "ModelId=" << m_anthropicModel << "\n";

    configFile.close();
    return true;
}

// Signal stubs

void QtGPT::setGeminiApiKey(const QString &key)
{
    m_geminiApiKey = key;
    emit settingsChanged();
}

void QtGPT::setGeminiModel(const QString &model)
{
    m_geminiModel = model;
    emit settingsChanged();
}

void QtGPT::setOpenaiApiKey(const QString &key)
{
    m_openaiApiKey = key;
    emit settingsChanged();
}

void QtGPT::setOpenaiModel(const QString &model)
{
    m_openaiModel = model;
    emit settingsChanged();
}

void QtGPT::setOpenaiBaseUrl(const QString &url)
{
    m_openaiBaseUrl = url;
    emit settingsChanged();
}

void QtGPT::setAnthropicApiKey(const QString &key)
{
    m_anthropicApiKey = key;
    emit settingsChanged();
}

void QtGPT::setAnthropicModel(const QString &model)
{
    m_anthropicModel = model;
    emit settingsChanged();
}

void QtGPT::setProvider(dp_provider_type_t provider)
{
    m_provider = provider;
    emit settingsChanged();
}

void QtGPT::setMaxHistoryMessages(int count)
{
    m_maxHistoryMessages = count;
    emit settingsChanged();
}

void QtGPT::setSystemPrompt(const QString &prompt)
{
    m_systemPrompt = prompt;
    emit settingsChanged();
}

void QtGPT::setPluginDirectory(const QString &dir)
{
    m_pluginDirectory = dir;
    m_pluginManager->setPluginDirectory(dir);
    emit settingsChanged();
}

void QtGPT::setAppendSystemPrompt(bool enable)
{
    m_appendSystemPrompt = enable;
    emit settingsChanged();
}

void QtGPT::setEnterSendsMessage(bool enable)
{
    m_enterSendsMessage = enable;
    emit settingsChanged();
}

QList<QMap<QString, QString>> QtGPT::chatHistory() const
{
    return m_chatHistory;
}

void QtGPT::addChatMessage(const QString &role, const QString &text, const QString &mime, const QString &base64)
{
    m_mutex.lock();
    QMap<QString, QString> msg;
    msg["role"] = role;
    msg["text"] = text;
    msg["mime"] = mime;
    msg["base64"] = base64;
    m_chatHistory.append(msg);
    
    if (m_chatHistory.count() > m_maxHistoryMessages) {
        m_chatHistory.removeFirst();
    }
    m_mutex.unlock();

    emit chatMessageAdded();
}

void QtGPT::clearChatHistory()
{
    m_mutex.lock();
    m_chatHistory.clear();
    m_mutex.unlock();
}

void QtGPT::loadPlugins()
{
    m_pluginManager->loadPlugins(m_pluginDirectory);
}

void QtGPT::loadSettingsSlot()
{
    loadSettings();
    
    // Update UI with loaded settings
    if (m_settingsDialog) {
        m_settingsDialog->populateSettings();
    }
}

void QtGPT::discardChangesSlot()
{
    // Discard changes - reload from saved config
    loadSettings();
    
    if (m_settingsDialog) {
        m_settingsDialog->populateSettings();
    }
}

void QtGPT::openConversationSlot()
{
    // Open conversation - would need file dialog implementation
    clearChatSlot();
}

void QtGPT::saveConversationSlot()
{
    // Save conversation - would need file dialog implementation
    // No need to clear chat on save, normally, but following original stub logic
}

void QtGPT::openSettingsSlot()
{
    if (m_settingsDialog) {
        m_settingsDialog->populateSettings();
        m_settingsDialog->showSettings();
    }
}

void QtGPT::clearChatSlot()
{
    emit clearChatSignal();
    clearChatHistory();
}

void QtGPT::quitSlot()
{
    if (m_mainWindow) {
        m_mainWindow->close();
    }
}

void QtGPT::retryRequestedSlot(int index)
{
    m_mutex.lock();
    if (index >= 0 && index < m_chatHistory.count()) {
        // Truncate history to this index + 1 (keep the message being retried)
        m_chatHistory = m_chatHistory.mid(0, index + 1);
        m_mutex.unlock();
        emit chatMessageAdded();
        
        // Trigger completion (re-run)
        qDebug() << "Retry triggered for message at index:" << index;
    } else {
        m_mutex.unlock();
    }
}

void QtGPT::editRequestedSlot(int index, const QString &text)
{
    m_mutex.lock();
    if (index >= 0 && index < m_chatHistory.count()) {
        // Truncate history to this index + 1
        m_chatHistory = m_chatHistory.mid(0, index + 1);
        // Update the message text
        m_chatHistory[index]["text"] = text;
        m_mutex.unlock();
        emit chatMessageAdded();
        
        // Trigger completion (re-run)
        qDebug() << "Edit & Retry triggered for message at index:" << index << "with text:" << text;
    } else {
        m_mutex.unlock();
    }
}
