#include "ChatWidget.h"
#include <QScrollBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QStatusBar>
#include <QMap>
#include <QVector>
#include <QImage>
#include <QByteArray>
#include <QMimeData>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegularExpression>
#include <QDesktopServices>

ChatWidget::ChatWidget(QWidget *parent)
    : QWidget(parent)
{
    initUI();
}

ChatWidget::~ChatWidget()
{
}

void ChatWidget::initUI()
{
    m_scrollArea = new QScrollArea(this);
    m_messagesContainer = new QWidget();
    m_messageLayout = new QVBoxLayout(m_messagesContainer);
    m_messageLayout->setSpacing(12);
    m_messageLayout->setContentsMargins(16, 16, 16, 16);
    
    
    m_scrollArea->setWidget(m_messagesContainer);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    m_thinkingLabel = new QLabel("Ready");
    m_thinkingLabel->setStyleSheet("QLabel { color: #888888; font-style: italic; }");
    m_thinkingLabel->setVisible(false);
    m_thinkingLabel->setParent(m_messagesContainer);
    m_thinkingLabel->hide();
    
    m_inputWidget = new QTextEdit(this);
    m_inputWidget->setPlaceholderText("Type a message...");
    m_inputWidget->setMinimumHeight(100);
    m_inputWidget->setAcceptRichText(false);
    
    m_sendButton = new QPushButton("Send");
    m_sendButton->setMaximumWidth(120);
    m_sendButton->setDefault(true);
    m_sendButton->setEnabled(false);
    
    m_attachButton = new QPushButton("Attach");
    m_attachButton->setMaximumWidth(100);
    
    m_imageLabel = new QLabel("");
    m_imageLabel->setVisible(false);
    m_imageLabel->setMaximumSize(200, 150);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    
    m_statusBar = new QStatusBar(this);
    m_statusBar->setStyleSheet("QStatusBar { background-color: #f0f0f0; padding: 4px; }");
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(8);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    m_mainLayout->addWidget(m_scrollArea);
    
    m_imageRowLayout = new QHBoxLayout();
    m_imageRowLayout->addWidget(m_imageLabel);
    m_imageRowLayout->addStretch();
    m_imageRowLayout->addWidget(new QPushButton("Remove", this));
    
    m_inputLayout = new QHBoxLayout();
    m_inputLayout->addLayout(m_imageRowLayout);
    m_inputLayout->addWidget(m_inputWidget, 1);
    m_inputLayout->addWidget(m_attachButton);
    m_inputLayout->addWidget(m_sendButton);
    
    m_mainLayout->addLayout(m_inputLayout);
    m_mainLayout->addWidget(m_statusBar);
    
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidget::handleSend);
    connect(m_attachButton, &QPushButton::clicked, this, &ChatWidget::handleImageAttach);
    connect(m_inputWidget, &QTextEdit::textChanged, this, [this]() {
        m_sendButton->setEnabled(!isEmptyInput());
    });
    
    m_sendButton->setFocusPolicy(Qt::ClickFocus);
}

QString ChatWidget::getInputText() const
{
    return m_inputWidget->toPlainText();
}

void ChatWidget::setInputText(const QString &text)
{
    m_inputWidget->setPlainText(text);
}

void ChatWidget::focusInput()
{
    m_inputWidget->setFocus();
}

bool ChatWidget::isEmptyInput() const
{
    return m_inputWidget->toPlainText().trimmed().isEmpty();
}

void ChatWidget::attachImage(const QString &file_path, const QString &mime_type, const QByteArray &base64_data)
{
    m_attachedImagePath = file_path;
    m_attachedImageMimeType = mime_type;
    m_attachedImageBase64 = base64_data;
    
    if (!base64_data.isEmpty()) {
        QString base64Str = QString::fromLatin1(base64_data);
        m_imageLabel->setPixmap(QPixmap::fromImage(QImage::fromData(QByteArray::fromBase64(base64Str.toLatin1()))));
        m_imageLabel->setVisible(true);
    }
}

void ChatWidget::detachImage()
{
    m_attachedImagePath.clear();
    m_attachedImageMimeType.clear();
    m_attachedImageBase64.clear();
    m_imageLabel->clear();
    m_imageLabel->setVisible(false);
}

bool ChatWidget::hasImageAttachment() const
{
    return !m_attachedImagePath.isEmpty();
}

QString ChatWidget::attachedImagePath() const
{
    return m_attachedImagePath;
}

QString ChatWidget::attachedImageMimeType() const
{
    return m_attachedImageMimeType;
}

QString ChatWidget::attachedImageBase64() const
{
    return QString::fromLatin1(m_attachedImageBase64.toBase64());
}

void ChatWidget::setStatusBarMessage(const QString &message)
{
    m_statusBar->showMessage(message, 5000);
}

void ChatWidget::setThinkingIndicator(bool thinking, bool generating)
{
    if (thinking) {
        m_thinkingLabel->setText("Thinking...");
        m_thinkingLabel->setVisible(true);
    } else if (generating) {
        m_thinkingLabel->setText("Generating...");
        m_thinkingLabel->setVisible(true);
    } else {
        m_thinkingLabel->setVisible(false);
    }
}

void ChatWidget::createMessageWidget(const QString &role, const QString &text, const QString &image_data)
{
    QWidget *msgWidget = new QWidget();
    msgWidget->setObjectName(role);
    
    QVBoxLayout *msgLayout = new QVBoxLayout(msgWidget);
    msgLayout->setSpacing(4);
    msgLayout->setContentsMargins(8, 4, 8, 4);
    
    QLabel *roleLabel = new QLabel("<b>" + role + ":</b>");
    roleLabel->setStyleSheet("QLabel { color: " + roleColor(role) + "; }");
    msgLayout->addWidget(roleLabel);
    
    QTextEdit *textWidget = new QTextEdit();
    textWidget->setPlainText(text);
    textWidget->setReadOnly(true);
    textWidget->setMaximumHeight(300);
    textWidget->setStyleSheet("QTextEdit { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 4px; padding: 8px; }");
    msgLayout->addWidget(textWidget);
    
 if (!image_data.isEmpty()) {
        QLabel *imageLabel = new QLabel();
        QByteArray decoded = QByteArray::fromBase64(image_data.toLatin1());
        imageLabel->setPixmap(QPixmap::fromImage(QImage::fromData(decoded)));
        imageLabel->setMaximumWidth(300);
        imageLabel->setAlignment(Qt::AlignCenter);
        msgLayout->addWidget(imageLabel);
    }
    
    m_messageWidgets.append(msgWidget);
    m_messageLayout->addWidget(msgWidget);
}

QString ChatWidget::roleColor(const QString &role)
{
    if (role == "user") return "#2E7BFF";
    if (role == "assistant") return "#4CAF50";
    if (role == "tool") return "#FF9800";
    if (role == "system") return "#9E9E9E";
    return "#000000";
}

void ChatWidget::renderHistory(const QVector<QMap<QString, QString>> &history)
{
    // Clear existing widgets
    for (QWidget *widget : m_messageWidgets) {
        widget->setParent(nullptr);
        widget->deleteLater();
    }
    m_messageWidgets.clear();
    
    for (const QMap<QString, QString> &msg : history) {
        QString role = msg.value("role");
        QString text = msg.value("text", "");
        QString image_data = msg.value("image_data", "");
        
        createMessageWidget(role, text, image_data);
    }
    
    scrollToBottom();
}

void ChatWidget::addMessage(const QString &role, const QString &text, const QString &mime_type, const QByteArray &base64_data)
{
    QWidget *msgWidget = new QWidget();
    msgWidget->setObjectName(role);
    
    QVBoxLayout *msgLayout = new QVBoxLayout(msgWidget);
    msgLayout->setSpacing(4);
    msgLayout->setContentsMargins(8, 4, 8, 4);
    
    QLabel *roleLabel = new QLabel("<b>" + role + ":</b>");
    roleLabel->setStyleSheet("QLabel { color: " + roleColor(role) + "; }");
    msgLayout->addWidget(roleLabel);
    
    QTextEdit *textWidget = new QTextEdit();
    textWidget->setPlainText(text);
    textWidget->setReadOnly(true);
    textWidget->setMaximumHeight(300);
    textWidget->setStyleSheet("QTextEdit { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 4px; padding: 8px; }");
    msgLayout->addWidget(textWidget);
    
   if (!base64_data.isEmpty()) {
         QString base64Str = QString::fromLatin1(base64_data);
         QLabel *imageLabel = new QLabel();
         QByteArray decoded = QByteArray::fromBase64(base64Str.toLatin1());
         imageLabel->setPixmap(QPixmap::fromImage(QImage::fromData(decoded)));
         imageLabel->setMaximumWidth(300);
         imageLabel->setAlignment(Qt::AlignCenter);
         msgLayout->addWidget(imageLabel);
     }
    
    m_messageWidgets.append(msgWidget);
    m_messageLayout->addWidget(msgWidget);
    scrollToBottom();
}

void ChatWidget::addThinkingMessage(const QString &text)
{
    QWidget *msgWidget = new QWidget();
    
    QVBoxLayout *msgLayout = new QVBoxLayout(msgWidget);
    msgLayout->setSpacing(4);
    msgLayout->setContentsMargins(8, 4, 8, 4);
    
    QLabel *roleLabel = new QLabel("<b>Assistant:</b>");
    roleLabel->setStyleSheet("QLabel { color: #4CAF50; }");
    msgLayout->addWidget(roleLabel);
    
    QTextEdit *textWidget = new QTextEdit();
    textWidget->setPlainText(text);
    textWidget->setReadOnly(true);
    textWidget->setMaximumHeight(300);
    textWidget->setStyleSheet("QTextEdit { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 4px; padding: 8px; }");
    msgLayout->addWidget(textWidget);
    
    m_messageWidgets.append(msgWidget);
    m_messageLayout->addWidget(msgWidget);
    scrollToBottom();
}

void ChatWidget::addToolCallMessage(const QString &tool_name, const QString &arguments)
{
    QWidget *msgWidget = new QWidget();
    
    QVBoxLayout *msgLayout = new QVBoxLayout(msgWidget);
    msgLayout->setSpacing(4);
    msgLayout->setContentsMargins(8, 4, 8, 4);
    
    QLabel *roleLabel = new QLabel("<b>Tool Call:</b>");
    roleLabel->setStyleSheet("QLabel { color: #FF9800; }");
    msgLayout->addWidget(roleLabel);
    
    QTextEdit *textWidget = new QTextEdit();
    textWidget->setPlainText("Tool: " + tool_name + "\nArguments: " + arguments);
    textWidget->setReadOnly(true);
    textWidget->setMaximumHeight(300);
    textWidget->setStyleSheet("QTextEdit { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 4px; padding: 8px; }");
    msgLayout->addWidget(textWidget);
    
    m_messageWidgets.append(msgWidget);
    m_messageLayout->addWidget(msgWidget);
    scrollToBottom();
}

void ChatWidget::addToolResultMessage(const QString &tool_call_id, const QString &result)
{
    QWidget *msgWidget = new QWidget();
    
    QVBoxLayout *msgLayout = new QVBoxLayout(msgWidget);
    msgLayout->setSpacing(4);
    msgLayout->setContentsMargins(8, 4, 8, 4);
    
    QLabel *roleLabel = new QLabel("<b>Tool Result:</b>");
    roleLabel->setStyleSheet("QLabel { color: #FF9800; }");
    msgLayout->addWidget(roleLabel);
    
    QTextEdit *textWidget = new QTextEdit();
    textWidget->setPlainText("ID: " + tool_call_id + "\nResult: " + result);
    textWidget->setReadOnly(true);
    textWidget->setMaximumHeight(300);
    textWidget->setStyleSheet("QTextEdit { background-color: #f5f5f5; border: 1px solid #ddd; border-radius: 4px; padding: 8px; }");
    msgLayout->addWidget(textWidget);
    
    m_messageWidgets.append(msgWidget);
    m_messageLayout->addWidget(msgWidget);
    scrollToBottom();
}

void ChatWidget::clearMessages()
{
    for (QWidget *widget : m_messageWidgets) {
        widget->setParent(nullptr);
        widget->deleteLater();
    }
    m_messageWidgets.clear();
}

void ChatWidget::scrollToBottom()
{
    QScrollBar *scrollBar = m_scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::sendMessage()
{
    if (!isEmptyInput()) {
        QString text = getInputText();
        detachImage();
        createMessageWidget("user", text);
        setInputText("");
        m_sendButton->setEnabled(false);
        scrollToBottom();
        emit sendMessageSignal(text);
    }
}

void ChatWidget::sendToolResult(const QString &tool_call_id, const QString &result)
{
    emit sendToolResultSignal(tool_call_id, result);
    emit toolResultRequested(tool_call_id, result);
}

void ChatWidget::attachImageRequested()
{
    emit attachImageRequestedSignal();
}

void ChatWidget::handleSend()
{
    sendMessage();
}

void ChatWidget::handleImageAttach()
{
    attachImageRequested();
}

void ChatWidget::handleImageRemove()
{
    detachImage();
}

void ChatWidget::show()
{
    QWidget::show();
}
