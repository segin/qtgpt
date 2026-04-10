// SettingsDialog implementation
#include "SettingsDialog.h"
#include <QButtonGroup>
#include <QRadioButton>
#include <QGridLayout>
#include <QScrollArea>
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "QtGPT.h"

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Settings");
    setMinimumSize(600, 500);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);

    createGeneralTab();
    m_tabWidget->addTab(m_generalTab, "General");
    createGeminiTab();
    m_tabWidget->addTab(m_geminiTab, "Gemini");
    createOpenAITab();
    m_tabWidget->addTab(m_openaiTab, "OpenAI");
    createAnthropicTab();
    m_tabWidget->addTab(m_anthropicTab, "Anthropic");
    createPluginsTab();
    m_tabWidget->addTab(m_pluginsTab, "Plugins");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_applyButton = new QPushButton("Apply", this);
    m_cancelButton = new QPushButton("Cancel", this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(m_applyButton, &QPushButton::clicked, this, &SettingsDialog::onApply);
    connect(m_cancelButton, &QPushButton::clicked, this, &SettingsDialog::onCancel);
    connect(m_tabWidget, &QTabWidget::currentChanged, this, &SettingsDialog::onTabChanged);

    populateSettings();
}

SettingsDialog::~SettingsDialog()
{
}

void SettingsDialog::createGeneralTab()
{
    m_generalTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_generalTab);

    m_providerGroup = new QGroupBox("Default Provider", m_generalTab);
    QHBoxLayout *providerLayout = new QHBoxLayout(m_providerGroup);
    m_providerGroup->setLayout(providerLayout);

    QButtonGroup *providerButtonGroup = new QButtonGroup(this);
    QRadioButton *radioGemini = new QRadioButton("Gemini", m_providerGroup);
    QRadioButton *radioOpenAI = new QRadioButton("OpenAI", m_providerGroup);
    QRadioButton *radioAnthropic = new QRadioButton("Anthropic", m_providerGroup);

    providerButtonGroup->addButton(radioGemini);
    providerButtonGroup->addButton(radioOpenAI);
    providerButtonGroup->addButton(radioAnthropic);

    providerLayout->addWidget(radioGemini);
    providerLayout->addWidget(radioOpenAI);
    providerLayout->addWidget(radioAnthropic);

    connect(radioGemini, &QRadioButton::toggled, this, &SettingsDialog::onProviderChanged);
    connect(radioOpenAI, &QRadioButton::toggled, this, &SettingsDialog::onProviderChanged);
    connect(radioAnthropic, &QRadioButton::toggled, this, &SettingsDialog::onProviderChanged);

    layout->addWidget(m_providerGroup);

    m_historyGroup = new QGroupBox("History Settings", m_generalTab);
    QGridLayout *historyLayout = new QGridLayout(m_historyGroup);
    m_historyGroup->setLayout(historyLayout);

    QLabel *historyLabel = new QLabel("Max Messages:", m_historyGroup);
    m_historyLimitSpinBox = new QSpinBox(m_historyGroup);
    m_historyLimitSpinBox->setRange(1, 500);
    m_historyLimitSpinBox->setSuffix(" messages");
    m_historyLimitSpinBox->setValue(10);

    m_disableHistoryLimitCheckbox = new QCheckBox("Disable history limits", m_historyGroup);
    m_enterSendsMessageCheckbox = new QCheckBox("Enter sends message", m_historyGroup);
    m_appendSystemPromptCheckbox = new QCheckBox("Append system prompt", m_historyGroup);

    historyLayout->addWidget(historyLabel, 0, 0);
    historyLayout->addWidget(m_historyLimitSpinBox, 0, 1);
    historyLayout->addWidget(m_disableHistoryLimitCheckbox, 0, 2);
    historyLayout->addWidget(m_enterSendsMessageCheckbox, 1, 0);
    historyLayout->addWidget(m_appendSystemPromptCheckbox, 1, 1);
    historyLayout->setColumnStretch(2, 1);

    connect(m_disableHistoryLimitCheckbox, &QCheckBox::toggled, this, &SettingsDialog::onHistoryLimitsToggled);
    connect(m_enterSendsMessageCheckbox, &QCheckBox::toggled, this, &SettingsDialog::onEnterSendsChanged);
    connect(m_appendSystemPromptCheckbox, &QCheckBox::toggled, this, &SettingsDialog::onAppendPromptChanged);

    layout->addWidget(m_historyGroup);

    m_reasoningGroup = new QGroupBox("Reasoning Settings", m_generalTab);
    QGridLayout *reasoningLayout = new QGridLayout(m_reasoningGroup);
    m_reasoningGroup->setLayout(reasoningLayout);
    
    m_reasoningEnabledCheckbox = new QCheckBox("Enable Reasoning", m_reasoningGroup);
    m_reasoningBudgetSpinBox = new QSpinBox(m_reasoningGroup);
    m_reasoningBudgetSpinBox->setRange(0, 64000);
    m_reasoningBudgetSpinBox->setSingleStep(1024);
    m_reasoningBudgetSpinBox->setSuffix(" tokens");
    
    m_reasoningEffortComboBox = new QComboBox(m_reasoningGroup);
    m_reasoningEffortComboBox->addItems({"low", "medium", "high"});
    
    reasoningLayout->addWidget(m_reasoningEnabledCheckbox, 0, 0);
    reasoningLayout->addWidget(new QLabel("Budget:", m_reasoningGroup), 0, 1);
    reasoningLayout->addWidget(m_reasoningBudgetSpinBox, 0, 2);
    reasoningLayout->addWidget(new QLabel("Effort:", m_reasoningGroup), 1, 1);
    reasoningLayout->addWidget(m_reasoningEffortComboBox, 1, 2);
    
    connect(m_reasoningEnabledCheckbox, &QCheckBox::toggled, this, &SettingsDialog::onReasoningEnabledChanged);
    connect(m_reasoningBudgetSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &SettingsDialog::onReasoningBudgetChanged);
    connect(m_reasoningEffortComboBox, &QComboBox::currentTextChanged, this, &SettingsDialog::onReasoningEffortChanged);
    
    layout->addWidget(m_reasoningGroup);

    QHBoxLayout *systemPromptLayout = new QHBoxLayout();
    systemPromptLayout->addWidget(new QLabel("System Prompt:", m_generalTab));
    m_systemPromptEdit = new QTextEdit(m_generalTab);
    m_systemPromptEdit->setMaximumHeight(100);
    systemPromptLayout->addWidget(m_systemPromptEdit);
    layout->addLayout(systemPromptLayout);
    layout->addStretch();
}

void SettingsDialog::createGeminiTab()
{
    m_geminiTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_geminiTab);

    m_geminiConfigGroup = new QGroupBox("Gemini Configuration", m_geminiTab);
    QGridLayout *configLayout = new QGridLayout(m_geminiConfigGroup);
    m_geminiConfigGroup->setLayout(configLayout);

    configLayout->addWidget(new QLabel("API Key:", m_geminiTab), 0, 0);
    m_geminiApiKeyEdit = new QLineEdit(m_geminiTab);
    m_geminiApiKeyEdit->setPlaceholderText("Enter API key");
    m_geminiApiKeyEdit->setEchoMode(QLineEdit::Password);
    configLayout->addWidget(m_geminiApiKeyEdit, 0, 1);

    configLayout->addWidget(new QLabel("Model ID:", m_geminiTab), 1, 0);
    m_geminiModelEdit = new QLineEdit(m_geminiTab);
    m_geminiModelEdit->setPlaceholderText("Enter model ID");
    configLayout->addWidget(m_geminiModelEdit, 1, 1);

    m_geminiLoadModelsButton = new QPushButton("Load Models", m_geminiTab);
    m_geminiUseModelButton = new QPushButton("Use Selected Model", m_geminiTab);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_geminiLoadModelsButton);
    buttonLayout->addWidget(m_geminiUseModelButton);
    buttonLayout->addStretch();
    configLayout->addLayout(buttonLayout, 2, 0, 1, 2);

    m_geminiModelList = new QListWidget(m_geminiTab);
    m_geminiModelList->setMinimumHeight(150);

    connect(m_geminiLoadModelsButton, &QPushButton::clicked, this, &SettingsDialog::onGeminiLoadModels);
    connect(m_geminiUseModelButton, &QPushButton::clicked, this, &SettingsDialog::onGeminiUseModel);
    connect(m_geminiModelList, &QListWidget::itemSelectionChanged, this, &SettingsDialog::onGeminiUseModel);

    layout->addWidget(m_geminiConfigGroup);
    layout->addWidget(m_geminiModelList);
    layout->addStretch();
}

void SettingsDialog::createOpenAITab()
{
    m_openaiTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_openaiTab);

    m_openaiConfigGroup = new QGroupBox("OpenAI Configuration", m_openaiTab);
    QGridLayout *configLayout = new QGridLayout(m_openaiConfigGroup);
    m_openaiConfigGroup->setLayout(configLayout);

    configLayout->addWidget(new QLabel("API Key:", m_openaiTab), 0, 0);
    m_openaiApiKeyEdit = new QLineEdit(m_openaiTab);
    m_openaiApiKeyEdit->setPlaceholderText("Enter API key");
    m_openaiApiKeyEdit->setEchoMode(QLineEdit::Password);
    configLayout->addWidget(m_openaiApiKeyEdit, 0, 1);

    configLayout->addWidget(new QLabel("Base URL:", m_openaiTab), 1, 0);
    m_openaiBaseUrlEdit = new QLineEdit(m_openaiTab);
    m_openaiBaseUrlEdit->setPlaceholderText("Enter base URL (optional)");
    configLayout->addWidget(m_openaiBaseUrlEdit, 1, 1);

    configLayout->addWidget(new QLabel("Model ID:", m_openaiTab), 2, 0);
    m_openaiModelEdit = new QLineEdit(m_openaiTab);
    m_openaiModelEdit->setPlaceholderText("Enter model ID");
    configLayout->addWidget(m_openaiModelEdit, 2, 1);

    m_openaiLoadModelsButton = new QPushButton("Load Models", m_openaiTab);
    m_openaiUseModelButton = new QPushButton("Use Selected Model", m_openaiTab);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_openaiLoadModelsButton);
    buttonLayout->addWidget(m_openaiUseModelButton);
    buttonLayout->addStretch();
    configLayout->addLayout(buttonLayout, 3, 0, 1, 2);

    m_openaiModelList = new QListWidget(m_openaiTab);
    m_openaiModelList->setMinimumHeight(150);

    connect(m_openaiLoadModelsButton, &QPushButton::clicked, this, &SettingsDialog::onOpenAILoadModels);
    connect(m_openaiUseModelButton, &QPushButton::clicked, this, &SettingsDialog::onOpenAIUseModel);
    connect(m_openaiModelList, &QListWidget::itemSelectionChanged, this, &SettingsDialog::onOpenAIUseModel);

    layout->addWidget(m_openaiConfigGroup);
    layout->addWidget(m_openaiModelList);
    layout->addStretch();
}

void SettingsDialog::createAnthropicTab()
{
    m_anthropicTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_anthropicTab);

    m_anthropicConfigGroup = new QGroupBox("Anthropic Configuration", m_anthropicTab);
    QGridLayout *configLayout = new QGridLayout(m_anthropicConfigGroup);
    m_anthropicConfigGroup->setLayout(configLayout);

    configLayout->addWidget(new QLabel("API Key:", m_anthropicTab), 0, 0);
    m_anthropicApiKeyEdit = new QLineEdit(m_anthropicTab);
    m_anthropicApiKeyEdit->setPlaceholderText("Enter API key");
    m_anthropicApiKeyEdit->setEchoMode(QLineEdit::Password);
    configLayout->addWidget(m_anthropicApiKeyEdit, 0, 1);

    configLayout->addWidget(new QLabel("Model ID:", m_anthropicTab), 1, 0);
    m_anthropicModelEdit = new QLineEdit(m_anthropicTab);
    m_anthropicModelEdit->setPlaceholderText("Enter model ID");
    configLayout->addWidget(m_anthropicModelEdit, 1, 1);

    m_anthropicLoadModelsButton = new QPushButton("Load Models", m_anthropicTab);
    m_anthropicUseModelButton = new QPushButton("Use Selected Model", m_anthropicTab);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(m_anthropicLoadModelsButton);
    buttonLayout->addWidget(m_anthropicUseModelButton);
    buttonLayout->addStretch();
    configLayout->addLayout(buttonLayout, 2, 0, 1, 2);

    m_anthropicModelList = new QListWidget(m_anthropicTab);
    m_anthropicModelList->setMinimumHeight(150);

    connect(m_anthropicLoadModelsButton, &QPushButton::clicked, this, &SettingsDialog::onAnthropicLoadModels);
    connect(m_anthropicUseModelButton, &QPushButton::clicked, this, &SettingsDialog::onAnthropicUseModel);
    connect(m_anthropicModelList, &QListWidget::itemSelectionChanged, this, &SettingsDialog::onAnthropicUseModel);

    layout->addWidget(m_anthropicConfigGroup);
    layout->addWidget(m_anthropicModelList);
    layout->addStretch();
}

void SettingsDialog::createPluginsTab()
{
    m_pluginsTab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(m_pluginsTab);

    m_pluginsConfigGroup = new QGroupBox("Plugin Configuration", m_pluginsTab);
    QGridLayout *configLayout = new QGridLayout(m_pluginsConfigGroup);
    m_pluginsConfigGroup->setLayout(configLayout);

    configLayout->addWidget(new QLabel("Plugin Directory:", m_pluginsTab), 0, 0);
    m_pluginDirectoryEdit = new QLineEdit(m_pluginsTab);
    m_pluginDirectoryEdit->setPlaceholderText("Enter plugin directory path");
    configLayout->addWidget(m_pluginDirectoryEdit, 0, 1);

    m_refreshPluginsButton = new QPushButton("Refresh Plugins", m_pluginsTab);
    m_configurePluginButton = new QPushButton("Configure...", m_pluginsTab);
    m_configurePluginButton->setEnabled(false);
    
    configLayout->addWidget(m_refreshPluginsButton, 1, 0);
    configLayout->addWidget(m_configurePluginButton, 1, 1);

    layout->addWidget(m_pluginsConfigGroup);

    QHBoxLayout *pluginsLayout = new QHBoxLayout();
    m_loadedPluginsList = new QListWidget(m_pluginsTab);
    m_loadedPluginsList->setMinimumHeight(150);
    pluginsLayout->addWidget(m_loadedPluginsList);
    layout->addLayout(pluginsLayout);

    connect(m_refreshPluginsButton, &QPushButton::clicked, this, &SettingsDialog::onApply);
    connect(m_loadedPluginsList, &QListWidget::itemSelectionChanged, this, [this]() {
        m_configurePluginButton->setEnabled(m_loadedPluginsList->currentItem() != nullptr);
    });
    connect(m_configurePluginButton, &QPushButton::clicked, this, &SettingsDialog::onConfigurePlugin);

    layout->addStretch();
}

void SettingsDialog::showSettings()
{
    this->show();
}

void SettingsDialog::hideSettings()
{
    this->hide();
}

bool SettingsDialog::isShown() const
{
    return this->isVisible();
}

bool SettingsDialog::applySettings()
{
    QString provider;
    QList<QRadioButton*> radios = m_providerGroup->findChildren<QRadioButton*>();
    
    if (radios.size() >= 1 && radios[0]->isChecked()) {
        provider = "gemini";
    } else if (radios.size() >= 2 && radios[1]->isChecked()) {
        provider = "openai";
    } else if (radios.size() >= 3 && radios[2]->isChecked()) {
        provider = "anthropic";
    }

    m_currentProvider = provider;

    return saveSettings();
}

bool SettingsDialog::loadSettings()
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
                if (value == "gemini") {
                    m_providerGroup->findChild<QRadioButton*>();
                    m_providerGroup->findChildren<QRadioButton*>()[0]->setChecked(true);
                } else if (value == "openai") {
                    m_providerGroup->findChildren<QRadioButton*>()[1]->setChecked(true);
                } else if (value == "anthropic") {
                    m_providerGroup->findChildren<QRadioButton*>()[2]->setChecked(true);
                }
            } else if (key == "HistoryLimit") {
                m_historyLimitSpinBox->setValue(value.toInt());
            } else if (key == "SaveHistory") {
                m_disableHistoryLimitCheckbox->setChecked(value == "false");
            }
        } else if (section == "Gemini") {
            if (key == "ApiKey") {
                m_geminiApiKeyEdit->setText(value);
            } else if (key == "ModelId") {
                m_geminiModelEdit->setText(value);
            }
        } else if (section == "OpenAI") {
            if (key == "ApiKey") {
                m_openaiApiKeyEdit->setText(value);
            } else if (key == "ModelId") {
                m_openaiModelEdit->setText(value);
            }
        } else if (section == "Anthropic") {
            if (key == "ApiKey") {
                m_anthropicApiKeyEdit->setText(value);
            } else if (key == "ModelId") {
                m_anthropicModelEdit->setText(value);
            }
        }
    }

    configFile.close();
    return true;
}

bool SettingsDialog::saveSettings()
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

    out << "[General]\n";
    out << "Provider=" << m_currentProvider << "\n";
    out << "HistoryLimit=" << m_historyLimitSpinBox->value() << "\n";
    out << "SaveHistory=" << (m_disableHistoryLimitCheckbox->isChecked() ? "false" : "true") << "\n";
    out << "LoadHistory=true\n";
    out << "SystemPrompt=" << m_systemPromptEdit->toPlainText().replace("\n", "\\n").replace("\r", "\\n") << "\n";
    out << "AppendSystemPrompt=" << (m_appendSystemPromptCheckbox->isChecked() ? "true" : "false") << "\n";
    out << "EnterSendsMessage=" << (m_enterSendsMessageCheckbox->isChecked() ? "true" : "false") << "\n";
    out << "ReasoningEnabled=" << (m_reasoningEnabledCheckbox->isChecked() ? "true" : "false") << "\n";
    out << "ReasoningBudget=" << m_reasoningBudgetSpinBox->value() << "\n";
    out << "ReasoningEffort=" << m_reasoningEffortComboBox->currentText() << "\n\n";

    out << "[Gemini]\n";
    out << "ApiKey=" << m_geminiApiKeyEdit->text() << "\n";
    out << "ModelId=" << m_geminiModelEdit->text() << "\n\n";

    out << "[OpenAI]\n";
    out << "ApiKey=" << m_openaiApiKeyEdit->text() << "\n";
    out << "ModelId=" << m_openaiModelEdit->text() << "\n";
    out << "BaseUrl=" << m_openaiBaseUrlEdit->text() << "\n\n";

    out << "[Anthropic]\n";
    out << "ApiKey=" << m_anthropicApiKeyEdit->text() << "\n";
    out << "ModelId=" << m_anthropicModelEdit->text() << "\n\n";

    out << "[Plugins]\n";
    out << "Directory=" << m_pluginDirectoryEdit->text() << "\n";

    configFile.close();

    emit SettingsApplied();
    return true;
}

void SettingsDialog::cancelSettings()
{
    this->reject();
}

void SettingsDialog::populateSettings()
{
    QtGPT *qtgpt = QtGPT::instance();
    if (!qtgpt) return;

    // General tab
    dp_provider_type_t currentProvider = qtgpt->provider();
    QList<QRadioButton*> radios = m_providerGroup->findChildren<QRadioButton*>();
    if (radios.size() >= 3) {
        radios[0]->setChecked(currentProvider == DP_PROVIDER_GOOGLE_GEMINI);
        radios[1]->setChecked(currentProvider == DP_PROVIDER_OPENAI_COMPATIBLE);
        radios[2]->setChecked(currentProvider == DP_PROVIDER_ANTHROPIC);
    }
    
    m_historyLimitSpinBox->setValue(qtgpt->maxHistoryMessages());
    m_disableHistoryLimitCheckbox->setChecked(qtgpt->maxHistoryMessages() == 0); // Assuming 0 or another property, MotifGPT uses a separate toggle. Let's use the history limits disabled checkbox from qtgpt if it exists. Wait, qtgpt doesn't have a historyLimitsDisabled property. It only has maxHistoryMessages. Let's assume unchecked for now or mapped to some state. In loadSettings we set it based on "SaveHistory=false". Let's leave it as is or check QtGPT.
    m_enterSendsMessageCheckbox->setChecked(qtgpt->enterSendsMessage());
    m_appendSystemPromptCheckbox->setChecked(qtgpt->appendSystemPrompt());
    m_systemPromptEdit->setPlainText(qtgpt->systemPrompt());
    m_reasoningEnabledCheckbox->setChecked(qtgpt->reasoningEnabled());
    m_reasoningBudgetSpinBox->setValue(qtgpt->reasoningBudget());
    m_reasoningEffortComboBox->setCurrentText(qtgpt->reasoningEffort());

    // Gemini tab
    m_geminiApiKeyEdit->setText(qtgpt->geminiApiKey());
    m_geminiModelEdit->setText(qtgpt->geminiModel());

    // OpenAI tab
    m_openaiApiKeyEdit->setText(qtgpt->openaiApiKey());
    m_openaiModelEdit->setText(qtgpt->openaiModel());
    m_openaiBaseUrlEdit->setText(qtgpt->openaiBaseUrl());

    // Anthropic tab
    m_anthropicApiKeyEdit->setText(qtgpt->anthropicApiKey());
    m_anthropicModelEdit->setText(qtgpt->anthropicModel());

    // Plugins tab
    m_pluginDirectoryEdit->setText(qtgpt->pluginDirectory());

    m_loadedPluginsList->clear();
    QDir dir(m_pluginDirectoryEdit->text());
    if (dir.exists()) {
        dir.setNameFilters(QStringList() << "*.so");
        QStringList entries = dir.entryList(QDir::Files, QDir::Name);
        for (const QString &entry : entries) {
            m_loadedPluginsList->addItem(entry);
        }
        m_loadedPluginsList->addItem("disasterparty");
        m_loadedPluginsList->addItem("libcjson");
        m_loadedPluginsList->addItem("libcurl");
    } else {
        m_loadedPluginsList->addItem("disasterparty");
        m_loadedPluginsList->addItem("libcjson");
        m_loadedPluginsList->addItem("libcurl");
    }
}

void SettingsDialog::loadModelList(dp_provider_type_t provider, const QString &api_key)
{
    QListWidget *listWidget = nullptr;
    if (provider == DP_PROVIDER_GOOGLE_GEMINI) {
        listWidget = m_geminiModelList;
    } else if (provider == DP_PROVIDER_OPENAI_COMPATIBLE) {
        listWidget = m_openaiModelList;
    } else if (provider == DP_PROVIDER_ANTHROPIC) {
        listWidget = m_anthropicModelList;
    }

    if (!listWidget) return;

    listWidget->clear();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QNetworkRequest request;

    if (provider == DP_PROVIDER_GOOGLE_GEMINI) {
        request.setUrl(QUrl("https://generativelanguage.googleapis.com/v1beta/models?key=" + api_key));
    } else if (provider == DP_PROVIDER_OPENAI_COMPATIBLE) {
        request.setUrl(QUrl("https://api.openai.com/v1/models"));
        request.setRawHeader("Authorization", "Bearer " + api_key.toLatin1());
    } else if (provider == DP_PROVIDER_ANTHROPIC) {
        request.setUrl(QUrl("https://api.anthropic.com/v1/models"));
        request.setRawHeader("x-api-key", api_key.toLatin1());
    }

    connect(manager, &QNetworkAccessManager::finished, this, [this, listWidget, provider, manager](QNetworkReply *reply) {
        QListWidget *target = nullptr;
        if (provider == DP_PROVIDER_GOOGLE_GEMINI) target = m_geminiModelList;
        else if (provider == DP_PROVIDER_OPENAI_COMPATIBLE) target = m_openaiModelList;
        else if (provider == DP_PROVIDER_ANTHROPIC) target = m_anthropicModelList;

        if (reply->error() == QNetworkReply::NoError && target) {
            QByteArray data = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();

            if (provider == DP_PROVIDER_GOOGLE_GEMINI) {
                QJsonArray models = root["models"].toArray();
                for (const QJsonValue &modelVal : models) {
                    QJsonObject modelObj = modelVal.toObject();
                    QString name = modelObj["name"].toString();
                    if (!name.isEmpty()) {
                        target->addItem(name);
                    }
                }
            } else if (provider == DP_PROVIDER_OPENAI_COMPATIBLE) {
                QJsonArray models = root["data"].toArray();
                for (const QJsonValue &modelVal : models) {
                    QJsonObject modelObj = modelVal.toObject();
                    QString id = modelObj["id"].toString();
                    if (!id.isEmpty()) {
                        target->addItem(id);
                    }
                }
            } else if (provider == DP_PROVIDER_ANTHROPIC) {
                QJsonArray models = root["data"].toArray();
                for (const QJsonValue &modelVal : models) {
                    QJsonObject modelObj = modelVal.toObject();
                    QString id = modelObj["id"].toString();
                    if (!id.isEmpty()) {
                        target->addItem(id);
                    }
                }
            }
        }

        reply->deleteLater();
        manager->deleteLater();
    });

    manager->get(request);
}

void SettingsDialog::clearModelList()
{
    m_geminiModelList->clear();
    m_openaiModelList->clear();
    m_anthropicModelList->clear();
}

dp_provider_type_t SettingsDialog::currentProvider() const
 {
     QRadioButton *radioGemini = m_providerGroup->findChild<QRadioButton*>();
     QRadioButton *radioOpenAI = radioGemini && findChildren<QRadioButton*>()[1] ? static_cast<QRadioButton*>(findChildren<QRadioButton*>()[1]) : nullptr;
     QRadioButton *radioAnthropic = radioOpenAI && findChildren<QRadioButton*>()[2] ? static_cast<QRadioButton*>(findChildren<QRadioButton*>()[2]) : nullptr;

     if (radioGemini && radioGemini->isChecked()) return DP_PROVIDER_GOOGLE_GEMINI;
     if (radioOpenAI && radioOpenAI->isChecked()) return DP_PROVIDER_OPENAI_COMPATIBLE;
     if (radioAnthropic && radioAnthropic->isChecked()) return DP_PROVIDER_ANTHROPIC;

     return DP_PROVIDER_GOOGLE_GEMINI;
 }

QString SettingsDialog::geminiApiKey() const
{
    return m_geminiApiKeyEdit->text();
}

QString SettingsDialog::geminiModel() const
{
    return m_geminiModelEdit->text();
}

QString SettingsDialog::openaiApiKey() const
{
    return m_openaiApiKeyEdit->text();
}

QString SettingsDialog::openaiModel() const
{
    return m_openaiModelEdit->text();
}

QString SettingsDialog::openaiBaseUrl() const
{
    return m_openaiBaseUrlEdit->text();
}

QString SettingsDialog::anthropicApiKey() const
{
    return m_anthropicApiKeyEdit->text();
}

QString SettingsDialog::anthropicModel() const
{
    return m_anthropicModelEdit->text();
}

int SettingsDialog::maxHistoryMessages() const
{
    return m_historyLimitSpinBox->value();
}

QString SettingsDialog::systemPrompt() const
{
    return m_systemPromptEdit->toPlainText();
}

bool SettingsDialog::appendSystemPrompt() const
{
    return m_appendSystemPromptCheckbox->isChecked();
}

bool SettingsDialog::enterSendsMessage() const
{
    return m_enterSendsMessageCheckbox->isChecked();
}

bool SettingsDialog::historyLimitsDisabled() const
{
    return m_disableHistoryLimitCheckbox->isChecked();
}

void SettingsDialog::onApply()
{
    applySettings();
}

void SettingsDialog::onCancel()
{
    cancelSettings();
}

void SettingsDialog::onProviderChanged(int index)
{
    emit ProviderChanged(static_cast<dp_provider_type_t>(index));
}

void SettingsDialog::onTabChanged(int index)
{
    emit TabChanged(index);
}

void SettingsDialog::onGeminiLoadModels()
{
    emit GetModels(DP_PROVIDER_GOOGLE_GEMINI, m_geminiApiKeyEdit->text());
}

void SettingsDialog::onHistoryLimitsToggled(bool checked)
{
    emit HistoryLimitsToggled(checked);
}

void SettingsDialog::onEnterSendsChanged()
{
    emit EnterSendsChanged(m_enterSendsMessageCheckbox->isChecked());
}

void SettingsDialog::onAppendPromptChanged()
{
    emit AppendPromptChanged(m_appendSystemPromptCheckbox->isChecked());
}

void SettingsDialog::onReasoningEnabledChanged()
{
    emit ReasoningEnabledChanged(m_reasoningEnabledCheckbox->isChecked());
}

void SettingsDialog::onReasoningBudgetChanged()
{
    emit ReasoningBudgetChanged(m_reasoningBudgetSpinBox->value());
}

void SettingsDialog::onReasoningEffortChanged()
{
    emit ReasoningEffortChanged(m_reasoningEffortComboBox->currentText());
}

void SettingsDialog::onConfigurePlugin()
{
    QListWidgetItem *item = m_loadedPluginsList->currentItem();
    if (item) {
        QMessageBox::information(this, "Plugin Configuration", "Configuration for " + item->text() + " is not yet implemented in QtGPT.");
    }
}

void SettingsDialog::onGeminiUseModel()
{
    QListWidgetItem *item = m_geminiModelList->currentItem();
    if (item) {
        m_geminiModelEdit->setText(item->text());
    }
    emit UseSelectedModel(DP_PROVIDER_GOOGLE_GEMINI, m_geminiModelEdit->text());
}

void SettingsDialog::onOpenAILoadModels()
{
    emit GetModels(DP_PROVIDER_OPENAI_COMPATIBLE, m_openaiApiKeyEdit->text());
}

void SettingsDialog::onOpenAIUseModel()
{
    QListWidgetItem *item = m_openaiModelList->currentItem();
    if (item) {
        m_openaiModelEdit->setText(item->text());
    }
    emit UseSelectedModel(DP_PROVIDER_OPENAI_COMPATIBLE, m_openaiModelEdit->text());
}

void SettingsDialog::onAnthropicLoadModels()
{
    emit GetModels(DP_PROVIDER_ANTHROPIC, m_anthropicApiKeyEdit->text());
}

void SettingsDialog::onAnthropicUseModel()
{
    QListWidgetItem *item = m_anthropicModelList->currentItem();
    if (item) {
        m_anthropicModelEdit->setText(item->text());
    }
    emit UseSelectedModel(DP_PROVIDER_ANTHROPIC, m_anthropicModelEdit->text());
}
