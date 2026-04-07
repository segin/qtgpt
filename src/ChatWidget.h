#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include <QMap>
#include <QVector>
#include <QByteArray>
#include <disasterparty.h>

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWidget(QWidget *parent = nullptr);
    ~ChatWidget();

    // Message display
    void addMessage(const QString &role, const QString &text, const QString &mime_type = QString(), const QByteArray &base64 = QByteArray());
    void addThinkingMessage(const QString &text);
    void addToolCallMessage(const QString &tool_name, const QString &arguments);
    void addToolResultMessage(const QString &tool_call_id, const QString &result);
    void clearMessages();

    // Input handling
    QString getInputText() const;
    void setInputText(const QString &text);
    void focusInput();
    bool isEmptyInput() const;

    // Image attachment
    void attachImage(const QString &file_path, const QString &mime_type, const QByteArray &base64_data);
    void detachImage();
    bool hasImageAttachment() const;
    QString attachedImagePath() const;
    QString attachedImageMimeType() const;
    QString attachedImageBase64() const;

    // Status bar
    void setStatusBarMessage(const QString &message);
    void setThinkingIndicator(bool thinking, bool generating);

    // Chat history
    void renderHistory(const QVector<QMap<QString, QString>> &history);
    void scrollToBottom();
    void createMessageWidget(const QString &role, const QString &text, const QString &image_data = QString());
    QString roleColor(const QString &role);

    // UI initialization
    void initUI();
    void show();
    void hide();
    void close();

public slots:
    void sendMessage();
    void sendToolResult(const QString &tool_call_id, const QString &result);
    void attachImageRequested();
    void handleSend();
    void handleImageAttach();
    void handleImageRemove();

signals:
    void sendMessageSignal(const QString &text);
    void sendToolResultSignal(const QString &tool_call_id, const QString &result);
    void attachImageRequestedSignal();
    void clearChatSignal();
    void toolResultRequested(const QString &tool_call_id, const QString &result);
    void clearChat();

private:
    QScrollArea *m_scrollArea;
    QWidget *m_messagesContainer;
    QVBoxLayout *m_messageLayout;
    QTextEdit *m_inputWidget;
    QPushButton *m_sendButton;
    QPushButton *m_attachButton;
    QLabel *m_imageLabel;
    QStatusBar *m_statusBar;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_inputLayout;
    QHBoxLayout *m_imageRowLayout;
    QList<QWidget*> m_messageWidgets;
    QLabel *m_thinkingLabel;
    QString m_attachedImagePath;
    QString m_attachedImageMimeType;
    QByteArray m_attachedImageBase64;
};

#endif // CHATWIDGET_H
