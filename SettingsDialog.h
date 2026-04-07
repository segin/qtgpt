#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <disasterparty.h>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

    // Show/hide dialog
    void showSettings();
    void hideSettings();
    bool isShown() const;

    // Apply settings
    bool applySettings();
    void cancelSettings();

    // Model loading
    void loadModelList(dp_provider_type_t provider, const QString &api_key);
    void clearModelList();

private slots:
    void onApply();
    void onCancel();
    void onProviderChanged(int index);
    void onTabChanged(int index);
    void onGeminiModelChanged();
    void onOpenAIModelChanged();
    void onAnthropicModelChanged();
    void onLoadModels();
    void onUseModelSelected();
    void onModelListError(const QString &error);
    void onHistoryLimitsToggled(bool checked);
    void onEnterSendsToggled(bool checked);
    void onAppendPromptToggled(bool checked);

private:
    QTabWidget *m_tabWidget;
    QWidget *m_generalTab;
    QWidget *m_geminiTab;
    QWidget *m_openaiTab;
    QWidget *m_anthropicTab;
    QWidget *m_systemPromptTab;
    QWidget *m_historyTab;
    QWidget *m_pluginsTab;

    // Radio buttons for providers
    QComboBox *m_providerComboBox;

    // Gemini tab
    QLineEdit *m_geminiApiKey;
    QLineEdit *m_geminiModel;
    QComboBox *m_geminiModelList;
    QPushButton *m_loadGeminiModels;

    // OpenAI tab
    QLineEdit *m_openaiApiKey;
    QLineEdit *m_openaiModel;
    QLineEdit *m_openaiBaseUrl;
    QComboBox *m_openaiModelList;
    QPushButton *m_loadOpenAIModels;

    // Anthropic tab
    QLineEdit *m_anthropicApiKey;
    QLineEdit *m_anthropicModel;
    QComboBox *m_anthropicModelList;
    QPushButton *m_loadAnthropicModels;

    // System prompt tab
    QTextEdit *m_systemPromptEdit;
    QCheckBox *m_appendSystemPrompt;

    // History tab
    QSpinBox *m_historyLimit;
    QCheckBox *m_historyLimitsDisabled;
    QCheckBox *m_enterSendsMessage;
    QCheckBox *m_appendDefaultSystemPrompt;

    // Plugins tab
    QLineEdit *m_pluginDirectory;
    QPushButton *m_refreshPlugins;
    QListWidget *m_pluginsList;

    // Dialog buttons
    QPushButton *m_applyButton;
    QPushButton *m_cancelButton;

    // Layouts
    QVBoxLayout *m_mainLayout;

    // Initialize UI components
    void initUI();
    void createGeneralTab();
    void createGeminiTab();
    void createOpenAITab();
    void createAnthropicTab();
    void createSystemPromptTab();
    void createHistoryTab();
    void createPluginsTab();
};

#endif // SETTINGS_DIALOG_H
