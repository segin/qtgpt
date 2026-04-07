#ifndef QTGPT_H
#define QTGPT_H

#include <QObject>
#include <QString>
#include <QMutex>
#include <QMap>
#include <QThread>
#include "disasterparty_wrapper.h"

class PipeHandler;
class PluginManager;
class SettingsDialog;
class ChatWidget;

class QtGPT : public QObject
    {
    public:
    explicit QtGPT(QObject *parent = nullptr);
    ~QtGPT();

    static QtGPT *instance();

    // Pipe handler for async communication
    PipeHandler *pipeHandler() const { return m_pipeHandler; }

    // Plugin manager for tools
    PluginManager *pluginManager() const { return m_pluginManager; }

    // Disasterparty context
    dp_context_t *context() const { return m_dpContext; }

    // Settings
    QString geminiApiKey() const { return m_geminiApiKey; }
    QString geminiModel() const { return m_geminiModel; }
    QString openaiApiKey() const { return m_openaiApiKey; }
    QString openaiModel() const { return m_openaiModel; }
    QString openaiBaseUrl() const { return m_openaiBaseUrl; }
    QString anthropicApiKey() const { return m_anthropicApiKey; }
    QString anthropicModel() const { return m_anthropicModel; }
    dp_provider_type_t provider() const { return m_provider; }
    int maxHistoryMessages() const { return m_maxHistoryMessages; }
    QString systemPrompt() const { return m_systemPrompt; }
    QString pluginDirectory() const { return m_pluginDirectory; }
    bool appendSystemPrompt() const { return m_appendSystemPrompt; }
    bool enterSendsMessage() const { return m_enterSendsMessage; }

    // Configuration
    public:
    void setGeminiApiKey(const QString &key);
    void setGeminiModel(const QString &model);
    void setOpenaiApiKey(const QString &key);
    void setOpenaiModel(const QString &model);
    void setOpenaiBaseUrl(const QString &url);
    void setAnthropicApiKey(const QString &key);
    void setAnthropicModel(const QString &model);
    void setProvider(dp_provider_type_t provider);
    void setMaxHistoryMessages(int count);
    void setSystemPrompt(const QString &prompt);
    void setPluginDirectory(const QString &dir);
    void setAppendSystemPrompt(bool enable);
    void setEnterSendsMessage(bool enable);

    // Initialization
    public:
    void initContext();
    void destroyContext();
    void loadPlugins();

    // Chat history (in memory for now)
    public:
    QList<QMap<QString, QString>> chatHistory() const;
    void addChatMessage(const QString &role, const QString &text, const QString &mime = QString(), const QString &base64 = QString());
    void clearChatHistory();

    // Main window
    void setMainWindow(ChatWidget *widget) { m_mainWindow = widget; }
    ChatWidget *mainWindow() const { return m_mainWindow; }

    // Settings dialog
    void setSettingsDialog(SettingsDialog *widget) { m_settingsDialog = widget; }
    SettingsDialog *settingsDialog() const { return m_settingsDialog; }

signals:
    void chatMessageAdded();
    void settingsChanged();

private:
    static QtGPT *s_instance;
    PipeHandler *m_pipeHandler;
    PluginManager *m_pluginManager;
    dp_context_t *m_dpContext;
    ChatWidget *m_mainWindow;
    SettingsDialog *m_settingsDialog;

    // Configuration
    dp_provider_type_t m_provider;
    QString m_geminiApiKey;
    QString m_geminiModel;
    QString m_openaiApiKey;
    QString m_openaiModel;
    QString m_openaiBaseUrl;
    QString m_anthropicApiKey;
    QString m_anthropicModel;
    int m_maxHistoryMessages;
    QString m_systemPrompt;
    QString m_pluginDirectory;
    bool m_appendSystemPrompt;
    bool m_enterSendsMessage;

    // Chat history
    QList<QMap<QString, QString>> m_chatHistory;

    // Mutex for thread safety
    QMutex m_mutex;
};

#endif // QTGPT_H
