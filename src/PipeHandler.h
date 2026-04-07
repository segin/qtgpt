#ifndef PIPEHANDLER_H
#define PIPEHANDLER_H

#include <QObject>
#include <QSocketNotifier>
#include <QMutex>
#include <QList>
#include <QVariant>

class PipeHandler : public QObject
{
    Q_OBJECT

public:
    explicit PipeHandler(QObject *parent = nullptr);
    ~PipeHandler();

    // Pipe management
    bool openPipe();
    int writeFd() const { return m_writeFd; }
    bool pipeOpen() const { return m_fd[0] != -1 && m_fd[1] != -1; }

    // Pipe message types
    enum MessageType {
        PIPE_MSG_TOKEN = 0,
        PIPE_MSG_THINKING = 1,
        PIPE_MSG_STREAM_END = 2,
        PIPE_MSG_ERROR = 3,
        PIPE_MSG_TOOL_RESULT = 4,
        PIPE_MSG_MODEL_LIST_ITEM = 5,
        PIPE_MSG_MODEL_LIST_END = 6,
        PIPE_MSG_MODEL_LIST_ERROR = 7
    };

    struct PipeMessage {
        MessageType type;
        QByteArray data;
        int tool_call_id;
        int tool_call_idx;
    };

    // Pipe management
    void closePipe();

    // Add handler for Qt event loop
    void startEventLoop();

    // Thread-safe write to pipe
    void sendMessage(MessageType type, const QByteArray &data = QByteArray(), int tool_call_id = -1, int tool_call_idx = -1);

signals:
    void tokenReceived(const QString &token);
    void thinkingStarted();
    void thinkingEnded();
    void streamEnded();
    void errorReceived(const QString &error);
    void toolResultReceived(int tool_call_id, int tool_call_idx, const QString &result);
    void modelListItemReceived(const QString &model);
    void modelListEnded();
    void modelListError(const QString &error);
    void pipeMessageReceived(const PipeMessage &message);

private:
    int m_fd[2];
    int m_writeFd;
    bool m_running;
    QSocketNotifier *m_notifier;
    QMutex m_mutex;

    // Event handler
    void handlePipeInput();

    static void pipeInputCallback(int fd, void *data);
};

#endif // PIPEHANDLER_H
