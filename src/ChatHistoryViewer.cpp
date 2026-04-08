#include "ChatHistoryViewer.h"
#include "ChatWidget.h"
#include "QtGPT.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QMessageBox>
#include <QDateTime>

ChatHistoryViewer::ChatHistoryViewer(QObject *parent)
    : QObject(parent)
{
}

ChatHistoryViewer::~ChatHistoryViewer()
{
}

bool ChatHistoryViewer::openConversation(const QString &filePath, QPointer<ChatWidget> chatWidget)
{
    if (!chatWidget) {
        QMessageBox::critical(nullptr, "Error", "Chat widget is not available.");
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Cannot open file: " + file.fileName());
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isEmpty()) {
        QMessageBox::critical(nullptr, "Error", "File is empty or invalid JSON.");
        return false;
    }

    QJsonObject root = doc.object();
    if (!root.contains("messages") || !root.contains("version")) {
        QMessageBox::critical(nullptr, "Error", "Invalid conversation file format.");
        return false;
    }

    QJsonArray messagesArray = root["messages"].toArray();
    QList<QMap<QString, QString>> history;

    for (const auto &msgVariant : messagesArray) {
        QJsonObject msgObj = msgVariant.toObject();
        QMap<QString, QString> msgMap;

        msgMap["role"] = msgObj.value("role").toString();
        msgMap["text"] = msgObj.value("text").toString();
        msgMap["mime"] = msgObj.value("mime").toString();
        msgMap["base64"] = msgObj.value("base64").toString();

        history.append(msgMap);
    }

    QtGPT::instance()->clearChatHistory();
    for (const auto &msg : history) {
        QtGPT::instance()->addChatMessage(
            msg["role"],
            msg["text"],
            msg["mime"],
            msg["base64"]
        );
    }

    auto chat = QtGPT::instance()->mainWindow();
    if (chat) {
        chat->renderHistory(QtGPT::instance()->chatHistory());
        chat->scrollToBottom();
    }

    return true;
}

bool ChatHistoryViewer::saveConversation(const QString &filePath, QPointer<ChatWidget> chatWidget)
{
    if (!chatWidget) {
        QMessageBox::critical(nullptr, "Error", "Chat widget is not available.");
        return false;
    }

    QList<QMap<QString, QString>> history = QtGPT::instance()->chatHistory();

    if (history.isEmpty()) {
        QMessageBox::warning(nullptr, "Warning", "No conversation history to save.");
        return false;
    }

    QJsonObject root;
    root["version"] = "1.0";
    root["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonArray messagesArray;
    for (const auto &msg : history) {
        QJsonObject msgObj;
        msgObj["role"] = msg["role"];
        msgObj["text"] = msg["text"];
        msgObj["mime"] = msg["mime"];
        msgObj["base64"] = msg["base64"];
        messagesArray.append(msgObj);
    }

    root["messages"] = messagesArray;

    QJsonDocument doc(root);
    QByteArray json = doc.toJson(QJsonDocument::Indented);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Error", "Cannot save file.");
        return false;
    }

    file.write(json);
    file.close();

    return true;
}

QJsonObject ChatHistoryViewer::messagesToJson(const QList<QMap<QString, QString>> &history)
{
    QJsonObject root;
    root["version"] = "1.0";
    root["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QJsonArray messagesArray;
    for (const auto &msg : history) {
        QJsonObject msgObj;
        msgObj["role"] = msg["role"];
        msgObj["text"] = msg["text"];
        msgObj["mime"] = msg["mime"];
        msgObj["base64"] = msg["base64"];
        messagesArray.append(msgObj);
    }

    root["messages"] = messagesArray;
    return root;
}

QList<QMap<QString, QString>> ChatHistoryViewer::jsonToMessages(const QJsonObject &data)
{
    QList<QMap<QString, QString>> history;

    if (data.contains("messages")) {
        QJsonArray messagesArray = data["messages"].toArray();

        for (const auto &msgVariant : messagesArray) {
            QJsonObject msgObj = msgVariant.toObject();
            QMap<QString, QString> msgMap;

            msgMap["role"] = msgObj.value("role").toString();
            msgMap["text"] = msgObj.value("text").toString();
            msgMap["mime"] = msgObj.value("mime").toString();
            msgMap["base64"] = msgObj.value("base64").toString();

            history.append(msgMap);
        }
    }

    return history;
}

bool ChatHistoryViewer::validateJsonStructure(const QJsonObject &data)
{
    if (!data.contains("version")) {
        return false;
    }

    if (!data.contains("messages")) {
        return false;
    }

    QJsonArray messagesArray = data["messages"].toArray();

    for (const auto &msgVariant : messagesArray) {
        QJsonObject msgObj = msgVariant.toObject();

        if (!msgObj.contains("role")) {
            return false;
        }

        QString role = msgObj.value("role").toString();
        if (role != "User" && role != "Assistant" && role != "Tool") {
            return false;
        }
    }

    return true;
}
