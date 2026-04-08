#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QListWidget>
#include <QLabel>
#include <QScrollArea>
#include <QGroupBox>
#include <QRadioButton>
#include <disasterparty.h>
#include <disasterparty_wrapper.h>

class QtGPT;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    // Show/hide dialog
    void showSettings();
    void hideSettings();
    bool isShown() const;

    // Apply settings
    bool applySettings();
    void cancelSettings();

    // Populate with current settings
    void populateSettings();

    // Model loading
    void loadModelList(dp_provider_type_t provider, const QString &api_key);
    void clearModelList();

    // Current settings
    dp_provider_type_t currentProvider() const;
    QString geminiApiKey() const;
    QString geminiModel() const;
    QString openaiApiKey() const;
    QString openaiModel() const;
    QString openaiBaseUrl() const;
    QString anthropicApiKey() const;
    QString anthropicModel() const;
    int maxHistoryMessages() const;
    QString systemPrompt() const;
    bool appendSystemPrompt() const;
    bool enterSendsMessage() const;
    bool historyLimitsDisabled() const;

    // Settings persistence
    bool loadSettings();
    bool saveSettings();

signals:
    void SettingsApplied();
    void ProviderChanged(dp_provider_type_t provider);
    void TabChanged(int index);
    void GetModels(dp_provider_type_t provider, const QString &api_key);
    void UseSelectedModel(dp_provider_type_t provider, const QString &model);
    void HistoryLimitsToggled(bool checked);
    void EnterSendsChanged(bool checked);
    void AppendPromptChanged(bool checked);

public slots:
    void onApply();
    void onCancel();
    void onProviderChanged(int index);
    void onTabChanged(int index);
    void onGeminiLoadModels();
    void onOpenAILoadModels();
    void onAnthropicLoadModels();
    void onGeminiUseModel();
    void onOpenAIUseModel();
    void onAnthropicUseModel();
    void onHistoryLimitsToggled(bool checked);
    void onEnterSendsChanged();
    void onAppendPromptChanged();
    void onConfigurePlugin();

public:
    QString m_currentProvider;

private:
    QListWidget *m_geminiModelList;
    QListWidget *m_openaiModelList;
    QListWidget *m_anthropicModelList;
    QTabWidget *m_tabWidget;
    QWidget *m_mainLayoutWidget;
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;

    // General tab
    QWidget *m_generalTab;
    QGroupBox *m_providerGroup;
    QComboBox *m_providerComboBox;
    QGroupBox *m_historyGroup;
    QSpinBox *m_historyLimitSpinBox;
    QCheckBox *m_disableHistoryLimitCheckbox;
    QCheckBox *m_enterSendsMessageCheckbox;
    QCheckBox *m_appendSystemPromptCheckbox;
    QTextEdit *m_systemPromptEdit;

    // Gemini tab
    QWidget *m_geminiTab;
    QGroupBox *m_geminiConfigGroup;
    QLineEdit *m_geminiApiKeyEdit;
    QLineEdit *m_geminiModelEdit;
    QPushButton *m_geminiLoadModelsButton;
    QPushButton *m_geminiUseModelButton;

    // OpenAI tab
    QWidget *m_openaiTab;
    QGroupBox *m_openaiConfigGroup;
    QLineEdit *m_openaiApiKeyEdit;
    QLineEdit *m_openaiModelEdit;
    QLineEdit *m_openaiBaseUrlEdit;
    QPushButton *m_openaiLoadModelsButton;
    QPushButton *m_openaiUseModelButton;

    // Anthropic tab
    QWidget *m_anthropicTab;
    QGroupBox *m_anthropicConfigGroup;
    QLineEdit *m_anthropicApiKeyEdit;
    QLineEdit *m_anthropicModelEdit;
    QPushButton *m_anthropicLoadModelsButton;
    QPushButton *m_anthropicUseModelButton;

    // Plugins tab
    QWidget *m_pluginsTab;
    QGroupBox *m_pluginsConfigGroup;
    QLineEdit *m_pluginDirectoryEdit;
    QPushButton *m_refreshPluginsButton;
    QPushButton *m_configurePluginButton;
    QListWidget *m_loadedPluginsList;

    // Initialize UI components
    void initUI();
    void createGeneralTab();
    void createGeminiTab();
    void createOpenAITab();
    void createAnthropicTab();
    void createPluginsTab();

    QtGPT *m_qtGPT;
};

#endif // SETTINGS_DIALOG_H
