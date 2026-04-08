#ifndef CHATHISTORYVIEWER_H
#define CHATHISTORYVIEWER_H

#include <QObject>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointer>
#include <QList>
#include <QMap>

class QtGPT;
class ChatWidget;

class ChatHistoryViewer : public QObject
{
    Q_OBJECT
public:
    explicit ChatHistoryViewer(QObject *parent = nullptr);
    ~ChatHistoryViewer();

    // Static methods for opening and saving conversations
    static bool openConversation(const QString &filePath, QPointer<ChatWidget> chatWidget);
    static bool saveConversation(const QString &filePath, QPointer<ChatWidget> chatWidget);

private:
    // Convert internal format to serialized format
    static QJsonObject messagesToJson(const QList<QMap<QString, QString>> &history);
    
    // Convert serialized format to internal format
    static QList<QMap<QString, QString>> jsonToMessages(const QJsonObject &data);
    
    // Validate JSON structure
    static bool validateJsonStructure(const QJsonObject &data);
};

#endif // CHATHISTORYVIEWER_H
