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
#include <QImage>
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
    public:
    QString getInputText() const;
    void setInputText(const QString &text);
    void focusInput();
    bool isEmptyInput() const;

    // Image attachment
    public:
    void attachImage(const QString &file_path, const QString &mime_type, const QByteArray &base64_data);
    void detachImage();
    bool hasImageAttachment() const;
    QString attachedImagePath() const;
    QString attachedImageMimeType() const;

    // Status bar
    public:
    void setStatusBarMessage(const QString &message);
    void setThinkingIndicator(bool thinking, bool generating);

    // Chat history
    public:
    void renderHistory(const QVector<QMap<QString, QString>> &history);
    void scrollToBottom();

 public:
    void show();
    void hide();
    void close();

  signals:
    void sendMessageSignal(const QString &text);
    void sendToolResultSignal(const QString &tool_call_json, int tool_call_id);
    void attachImageRequestedSignal();

  public slots:
    void sendMessage();
    void sendToolResult(const QString &tool_call_json, int tool_call_id);
    void attachImageRequested();

  private:
    QScrollArea *m_scrollArea;
    QWidget *m_messagesContainer;
    QTextEdit *m_inputWidget;
    QPushButton *m_sendButton;
    QPushButton *m_attachButton;
    QLabel *m_imageLabel;
    QStatusBar *m_statusBar;

    // Message widgets
    QList<QLabel*> m_messageWidgets;
    QLabel *m_thinkingLabel;

    // Current image state
    QString m_attachedImagePath;
    QString m_attachedImageMimeType;
    QByteArray m_attachedImageBase64;

    // Layouts
    QVBoxLayout *m_mainLayout;
    QVBoxLayout *m_messageLayout;
    QHBoxLayout *m_inputLayout;

    // Initialize UI components
    public:
    void initUI();
    void createMessageWidget();

  public slots:
    void handleSend();
    void handleImageAttach();
    void handleImageRemove();
};

#endif // CHATWIDGET_H
