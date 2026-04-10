#include "ChatWidget.h"
#include "ChatHistoryViewer.h"
#include <QScrollBar>
#include <QMessageBox>
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
#include <QApplication>
#include <QClipboard>
#include <QStyle>
#include <QKeyEvent>
#include <QEvent>

#include <QSplitter>

ChatWidget::ChatWidget(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
}

ChatWidget::~ChatWidget()
{
}

void ChatWidget::setMenuBar(QMenuBar *menuBar)
{
    QMainWindow::setMenuBar(menuBar);
}

void ChatWidget::initUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QSplitter *splitter = new QSplitter(Qt::Vertical, centralWidget);

    m_scrollArea = new QScrollArea(splitter);
    m_messagesContainer = new QWidget();
    m_messageLayout = new QVBoxLayout(m_messagesContainer);
    m_messageLayout->setSpacing(16);
    m_messageLayout->setContentsMargins(16, 16, 16, 16);
    m_messageLayout->addStretch(1);
    
    m_scrollArea->setWidget(m_messagesContainer);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    m_thinkingLabel = new QLabel("Ready");
    m_thinkingLabel->setStyleSheet("QLabel { color: #888888; font-style: italic; }");
    m_thinkingLabel->setVisible(false);
    
    QWidget *inputContainer = new QWidget(splitter);
    QVBoxLayout *inputVLayout = new QVBoxLayout(inputContainer);
    inputVLayout->setContentsMargins(8, 8, 8, 8);
    inputVLayout->setSpacing(4);
    
    m_imageLabel = new QLabel("", inputContainer);
    m_imageLabel->setVisible(false);
    m_imageLabel->setMaximumSize(200, 150);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    
    m_imageRowLayout = new QHBoxLayout();
    m_imageRowLayout->addWidget(m_imageLabel);
    m_imageRowLayout->addStretch();
    
    QHBoxLayout *inputRowLayout = new QHBoxLayout();
    inputRowLayout->setSpacing(4);
    
    m_attachButton = new QPushButton(inputContainer);
    m_attachButton->setIcon(QIcon::fromTheme("mail-attachment", style()->standardIcon(QStyle::SP_FileIcon)));
    m_attachButton->setToolTip("Attach Image");
    m_attachButton->setFixedSize(32, 32);
    m_attachButton->setFlat(true);
    
    m_inputWidget = new QTextEdit(inputContainer);
    m_inputWidget->setPlaceholderText("Type a message...");
    m_inputWidget->setAcceptRichText(false);
    m_inputWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_inputWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_inputWidget->installEventFilter(this);
    m_inputWidget->setFixedHeight(36);
    
    m_sendButton = new QPushButton(inputContainer);
    m_sendButton->setIcon(QIcon::fromTheme("mail-send", style()->standardIcon(QStyle::SP_ArrowRight)));
    m_sendButton->setToolTip("Send Message");
    m_sendButton->setFixedSize(32, 32);
    m_sendButton->setFlat(true);
    m_sendButton->setEnabled(false);
    
    inputRowLayout->addWidget(m_attachButton, 0, Qt::AlignBottom);
    inputRowLayout->addWidget(m_inputWidget, 1, Qt::AlignBottom);
    inputRowLayout->addWidget(m_sendButton, 0, Qt::AlignBottom);
    
    inputVLayout->addLayout(m_imageRowLayout);
    inputVLayout->addLayout(inputRowLayout);
    
    splitter->addWidget(m_scrollArea);
    splitter->addWidget(inputContainer);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    mainLayout->addWidget(splitter);
    
    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);
    m_statusBar->setStyleSheet("QStatusBar { background-color: #f0f0f0; padding: 4px; }");
    
    connect(m_sendButton, &QPushButton::clicked, this, &ChatWidget::handleSend);
    connect(m_attachButton, &QPushButton::clicked, this, &ChatWidget::handleImageAttach);
    connect(m_inputWidget, &QTextEdit::textChanged, this, [this]() {
        m_sendButton->setEnabled(!isEmptyInput());
        adjustInputHeight();
    });
    
    m_sendButton->setFocusPolicy(Qt::ClickFocus);
}

bool ChatWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_inputWidget && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (keyEvent->modifiers() & Qt::ShiftModifier) {
                m_inputWidget->insertPlainText("\n");
                return true;
            } else {
                handleSend();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void ChatWidget::adjustInputHeight()
{
    int lineCount = m_inputWidget->document()->lineCount();
    int newHeight = qMin(150, qMax(36, (int)(lineCount * 20 + 16)));
    if (m_inputWidget->height() != newHeight) {
        m_inputWidget->setFixedHeight(newHeight);
    }
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

void ChatWidget::createMessageWidget(const QString &role, const QString &text, const QString &image_data, int index)
{
    QWidget *msgWidget = new QWidget(m_messagesContainer);
    msgWidget->setObjectName(role);
    msgWidget->setMaximumWidth(400);
    
    QVBoxLayout *msgLayout = new QVBoxLayout(msgWidget);
    msgLayout->setSpacing(2);
    msgLayout->setContentsMargins(8, 4, 8, 4);
    
    QLabel *roleLabel = new QLabel("<b>" + role.toUpper() + "</b>");
    roleLabel->setStyleSheet("QLabel { color: " + roleColor(role) + "; font-size: 10px; }");
    msgLayout->addWidget(roleLabel, 0, (role == "user") ? Qt::AlignRight : Qt::AlignLeft);
    
    QTextEdit *textWidget = new QTextEdit();
    textWidget->setObjectName("messageText");
    if (role == "user") {
        textWidget->setPlainText(text);
    } else {
        textWidget->setMarkdown(text);
    }
    textWidget->setReadOnly(true);
    textWidget->setFrameStyle(QFrame::NoFrame);
    textWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    // Auto-height for text widget
    textWidget->document()->setTextWidth(380); // max width minus padding
    int docHeight = textWidget->document()->size().height();
    textWidget->setFixedHeight(docHeight + 10);
    
    textWidget->setStyleSheet("QTextEdit { background: transparent; border: none; padding: 0; }");
    
    if (role == "user") {
        QTextOption opt = textWidget->document()->defaultTextOption();
        opt.setAlignment(Qt::AlignRight);
        textWidget->document()->setDefaultTextOption(opt);
    }
    
    msgLayout->addWidget(textWidget, 0, (role == "user") ? Qt::AlignRight : Qt::AlignLeft);
    
    if (!image_data.isEmpty()) {
        QLabel *imageLabel = new QLabel();
        QByteArray decoded = QByteArray::fromBase64(image_data.toLatin1());
        imageLabel->setPixmap(QPixmap::fromImage(QImage::fromData(decoded)).scaledToWidth(300, Qt::SmoothTransformation));
        imageLabel->setAlignment((role == "user") ? Qt::AlignRight : Qt::AlignLeft);
        msgLayout->addWidget(imageLabel, 0, (role == "user") ? Qt::AlignRight : Qt::AlignLeft);
    }

    QHBoxLayout *footerLayout = new QHBoxLayout();
    footerLayout->setSpacing(4);
    footerLayout->setContentsMargins(0, 0, 0, 0);
    
    if (role == "user") {
        footerLayout->addStretch();
        
        QPushButton *editBtn = new QPushButton();
        editBtn->setIcon(QIcon::fromTheme("document-edit", style()->standardIcon(QStyle::SP_FileDialogContentsView))); 
        editBtn->setToolTip("Edit");
        editBtn->setFixedSize(24, 24);
        editBtn->setFlat(true);
        footerLayout->addWidget(editBtn);
        // ... connect logic kept same
        connect(editBtn, &QPushButton::clicked, this, [this, index, text, msgWidget, msgLayout, textWidget]() {
            // Replace text widget with an editable one
            textWidget->setReadOnly(false);
            textWidget->setFocus();
            textWidget->setStyleSheet("QTextEdit { background: white; border: 1px solid #ccc; padding: 4px; }");
            
            QTextOption opt = textWidget->document()->defaultTextOption();
            opt.setAlignment(Qt::AlignLeft);
            textWidget->document()->setDefaultTextOption(opt);
            
            QPushButton *saveBtn = new QPushButton("Save");
            QPushButton *cancelBtn = new QPushButton("Cancel");
            QHBoxLayout *editButtons = new QHBoxLayout();
            editButtons->addWidget(saveBtn);
            editButtons->addWidget(cancelBtn);
            editButtons->addStretch();
            msgLayout->addLayout(editButtons);
            
            connect(saveBtn, &QPushButton::clicked, this, [this, index, textWidget]() {
                onEditClicked(index, textWidget->toPlainText());
            });
            connect(cancelBtn, &QPushButton::clicked, this, [this]() {
                // Just re-render to cancel
                emit clearChat(); 
            });
        });

        QPushButton *retryBtn = new QPushButton();
        retryBtn->setIcon(QIcon::fromTheme("view-refresh", style()->standardIcon(QStyle::SP_BrowserReload)));
        retryBtn->setToolTip("Retry");
        retryBtn->setFixedSize(24, 24);
        retryBtn->setFlat(true);
        footerLayout->addWidget(retryBtn);
        connect(retryBtn, &QPushButton::clicked, this, [this, index]() {
            onRetryClicked(index);
        });
    }
    
    QPushButton *copyBtn = new QPushButton();
    copyBtn->setIcon(QIcon::fromTheme("edit-copy", style()->standardIcon(QStyle::SP_DialogSaveButton))); 
    copyBtn->setToolTip("Copy");
    copyBtn->setFixedSize(24, 24);
    copyBtn->setFlat(true);
    footerLayout->addWidget(copyBtn);
    connect(copyBtn, &QPushButton::clicked, this, [this, text]() {
        onCopyClicked(text);
    });
    
    if (role != "user") {
        footerLayout->addStretch();
    }
    
    msgLayout->addLayout(footerLayout);
    
    m_messageWidgets.append(msgWidget);
    Qt::Alignment alignment = (role == "user") ? Qt::AlignRight : Qt::AlignLeft;
    m_messageLayout->insertWidget(m_messageLayout->count() - 1, msgWidget, 0, alignment);
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
    // Fast path for streaming updates
    if (history.size() == m_messageWidgets.size() && !history.isEmpty()) {
        const QMap<QString, QString> &msg = history.last();
        QWidget *lastWidget = m_messageWidgets.last();
        QTextEdit *textWidget = lastWidget->findChild<QTextEdit*>("messageText");
        if (textWidget) {
            QString text = msg.value("text", "");
            QString role = msg.value("role", "");
            if (role == "user") {
                textWidget->setPlainText(text);
            } else {
                textWidget->setMarkdown(text);
            }
            int docHeight = textWidget->document()->size().height();
            textWidget->setFixedHeight(docHeight + 10);
            scrollToBottom();
            return;
        }
    }

    // Clear existing widgets
    for (QWidget *widget : m_messageWidgets) {
        widget->setParent(nullptr);
        widget->deleteLater();
    }
    m_messageWidgets.clear();
    
    for (int i = 0; i < history.size(); ++i) {
        const QMap<QString, QString> &msg = history[i];
        QString role = msg.value("role");
        QString text = msg.value("text", "");
        QString image_data = msg.value("image_data", "");
        
        createMessageWidget(role, text, image_data, i);
    }
    
    scrollToBottom();
}

void ChatWidget::addMessage(const QString &role, const QString &text, const QString &mime_type, const QByteArray &base64_data)
{
    Q_UNUSED(mime_type);
    QString base64Str;
    if (!base64_data.isEmpty()) {
        base64Str = QString::fromLatin1(base64_data.toBase64());
    }
    
    createMessageWidget(role, text, base64Str, m_messageWidgets.size());
    scrollToBottom();
}

void ChatWidget::addThinkingMessage(const QString &text)
{
    createMessageWidget("assistant", text, QString(), m_messageWidgets.size());
    scrollToBottom();
}

void ChatWidget::addToolCallMessage(const QString &tool_name, const QString &arguments)
{
    createMessageWidget("tool", "Tool: " + tool_name + "\nArguments: " + arguments, QString(), m_messageWidgets.size());
    scrollToBottom();
}

void ChatWidget::addToolResultMessage(const QString &tool_call_id, const QString &result)
{
    createMessageWidget("tool", "ID: " + tool_call_id + "\nResult: " + result, QString(), m_messageWidgets.size());
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

void ChatWidget::onEditClicked(int index, const QString &text)
{
    emit editRequested(index, text);
}

void ChatWidget::onRetryClicked(int index)
{
    emit retryRequested(index);
}

void ChatWidget::onCopyClicked(const QString &text)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(text);
    setStatusBarMessage("Copied to clipboard");
}

void ChatWidget::show()
{
    QWidget::show();
}

void ChatWidget::close()
{
    QWidget::close();
}


 
