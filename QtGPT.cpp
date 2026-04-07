#include "QtGPT.h"
#include "PipeHandler.h"
#include "PluginManager.h"
#include <QCoreApplication>

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

// Forward declarations for utils
class Utils;
QString Utils::getConfigPath(const QString &filename) { return QString(); }
QString Utils::getCachePath(const QString &filename) { return QString(); }
bool Utils::ensureConfigDirExists() { return true; }
QString Utils::getUserConfigDir() { return QString(); }
QString Utils::base64Encode(const QByteArray &data) { return QString(); }
QByteArray Utils::base64Decode(const QString &data) { return QByteArray(); }
bool Utils::readFileToBuffer(const QString &path, QByteArray &buffer, qint64 &size) { return false; }
QString Utils::readFileToString(const QString &path) { return QString(); }
QString Utils::getImageMimeType(const QByteArray &data) { return QString(); }
bool Utils::isImageSupported(const QString &mime) { return false; }
QString Utils::serializeMessages(const QStringList &messages) { return QString(); }
QStringList Utils::deserializeMessages(const QString &json) { return QStringList(); }
QString Utils::escapeJson(const QString &str) { return QString(); }
QString Utils::unescapeJson(const QString &str) { return QString(); }
QString Utils::generateSystemPrompt(const QString &custom_prompt, const QString &system_prompt, bool append_tools) { return QString(); }
QString Utils::generateDefaultSystemPrompt() { return QString(); }
int Utils::countTokens(const QString &text) { return 0; }
bool Utils::isValidModelName(const QString &model) { return true; }
bool Utils::isValidApiKey(const QString &key) { return !key.isEmpty(); }
QString Utils::mapMimeType(const QString &extension) { return QString(); }
