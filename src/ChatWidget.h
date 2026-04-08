#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QMainWindow>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include <QMenuBar>
#include <QSplitter>
#include <QMap>
#include <QVector>
#include <QByteArray>
#include <disasterparty.h>

class MenuBar;

class ChatWidget : public QMainWindow
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
    void createMessageWidget(const QString &role, const QString &text, const QString &image_data = QString(), int index = -1);
    QString roleColor(const QString &role);

    // UI initialization
    void initUI();
    void setMenuBar(QMenuBar *menuBar);
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
    void onRetryClicked(int index);
    void onCopyClicked(const QString &text);
    void onEditClicked(int index, const QString &text);

signals:
    void sendMessageSignal(const QString &text);
    void sendToolResultSignal(const QString &tool_call_id, const QString &result);
    void attachImageRequestedSignal();
    void clearChatSignal();
    void toolResultRequested(const QString &tool_call_id, const QString &result);
    void clearChat();
    void openConversation();
    void saveConversation();
    void retryRequested(int index);
    void editRequested(int index, const QString &text);
    void copyRequested(const QString &text);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void adjustInputHeight();
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
    QSpacerItem *m_messageSpacer;
    QLabel *m_thinkingLabel;
    QString m_attachedImagePath;
    QString m_attachedImageMimeType;
    QByteArray m_attachedImageBase64;
    QMenuBar *m_menuBar;
};

#endif // CHATWIDGET_H
