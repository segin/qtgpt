#include "ChatWidget.h"
#include <QScrollBar>

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
    m_messageLayout->setSpacing(10);
    m_messageLayout->setContentsMargins(10, 10, 10, 10);
    
    m_scrollArea->setWidget(m_messagesContainer);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    m_inputWidget = new QTextEdit(this);
    m_inputWidget->setPlaceholderText("Type a message...");
    
    m_sendButton = new QPushButton("Send", this);
    m_attachButton = new QPushButton("Attach", this);
    
    m_imageLabel = new QLabel(this);
    m_imageLabel->setVisible(false);
    
    m_statusBar = new QStatusBar(this);
    
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(5);
    
    m_inputLayout = new QHBoxLayout();
    m_inputLayout->addWidget(m_attachButton);
    m_inputLayout->addWidget(m_imageLabel);
    m_inputLayout->addStretch();
    m_inputLayout->addWidget(m_sendButton);
    
    m_mainLayout->addWidget(m_scrollArea);
    m_mainLayout->addWidget(m_inputWidget);
    m_mainLayout->addLayout(m_inputLayout);
    m_mainLayout->addWidget(m_statusBar);
    
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidget::handleSend);
    connect(m_attachButton, &QPushButton::clicked, this, &ChatWidget::handleImageAttach);
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
    m_imageLabel->setVisible(true);
}

void ChatWidget::detachImage()
{
    m_attachedImagePath.clear();
    m_attachedImageMimeType.clear();
    m_attachedImageBase64.clear();
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

void ChatWidget::setStatusBarMessage(const QString &message)
{
    m_statusBar->showMessage(message, 5000);
}

void ChatWidget::setThinkingIndicator(bool thinking, bool generating)
{
    QString status = "Ready";
    if (thinking) {
        status = "Thinking...";
    } else if (generating) {
        status = "Generating...";
    }
    setStatusBarMessage(status);
}

void ChatWidget::renderHistory(const QVector<QMap<QString, QString>> &history)
{
    // Clear existing widgets
    QList<QWidget*> widgets;
    for (int i = 0; i < m_messageWidgets.count(); ++i) {
        QWidget *widget = m_messageWidgets[i];
        widget->setParent(nullptr);
        widgets.append(widget);
    }
    
    // Rebuild widgets
    for (int i = 0; i < m_messageWidgets.count(); ++i) {
        QWidget *widget = widgets[i];
        delete widget;
    }
    m_messageWidgets.clear();
    
    for (const QMap<QString, QString> &msg : history) {
        QLabel *label = new QLabel(this);
        QString role = msg.value("role");
        QString text = msg.value("text");
        
        if (role == "user") {
            label->setText("<b>User:</b> " + text);
            label->setStyleSheet("QLabel { color: #2E7BFF; }");
        } else if (role == "assistant") {
            label->setText("<b>Assistant:</b> " + text);
            label->setStyleSheet("QLabel { color: #4CAF50; }");
        } else {
            label->setText("Message");
        }
        
        label->setWordWrap(true);
        m_messageWidgets.append(label);
        m_messageLayout->addWidget(label);
    }
    
    scrollToBottom();
}

void ChatWidget::scrollToBottom()
{
    QScrollBar *scrollBar = m_scrollArea->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void ChatWidget::sendMessage()
{
    emit sendMessage();
}

void ChatWidget::sendToolResult(const QString &tool_call_json, int tool_call_id)
{
    emit sendToolResult(tool_call_json, tool_call_id);
}

void ChatWidget::attachImageRequested()
{
    emit attachImageRequested();
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
