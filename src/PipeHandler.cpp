// Pipe handler for async communication (stub)
#include "PipeHandler.h"
#include <unistd.h>
#include <QByteArray>

PipeHandler::PipeHandler(QObject *parent)
    : QObject(parent)
{
    m_fd[0] = -1;
    m_fd[1] = -1;
    m_writeFd = -1;
    m_notifier = nullptr;
    m_running = false;
}

PipeHandler::~PipeHandler()
{
    closePipe();
}

bool PipeHandler::openPipe()
{
    if (pipe(m_fd) != 0) {
        return false;
    }
    m_writeFd = m_fd[1];
    m_notifier = new QSocketNotifier(m_fd[0], QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, &PipeHandler::handlePipeInput);
    return true;
}

void PipeHandler::closePipe()
{
    if (m_fd[0] != -1) {
        close(m_fd[0]);
        m_fd[0] = -1;
    }
    if (m_fd[1] != -1) {
        close(m_fd[1]);
        m_fd[1] = -1;
    }
    delete m_notifier;
    m_notifier = nullptr;
}

void PipeHandler::startEventLoop()
{
    // Qt event loop handles this via QSocketNotifier
    m_running = true;
}

void PipeHandler::sendMessage(MessageType type, const QString &data, int tool_call_id, int tool_call_idx)
{
    m_mutex.lock();
    PipeMessage msg;
    msg.type = static_cast<int>(type);
    msg.data = data.toUtf8();
    msg.tool_call_id = tool_call_id;
    msg.tool_call_idx = tool_call_idx;
    ssize_t written = write(m_writeFd, &msg, sizeof(msg));
    m_mutex.unlock();
    if (written < 0) {
        // Handle error
    }
}

void PipeHandler::handlePipeInput()
{
    PipeMessage msg;
    ssize_t read = read(m_fd[0], &msg, sizeof(msg));
    if (read > 0) {
        emit pipeMessageReceived(msg);
        
        switch (msg.type) {
            case PIPE_MSG_TOKEN:
                emit tokenReceived(QString::fromUtf8(msg.data));
                break;
            case PIPE_MSG_THINKING:
                emit thinkingStarted();
                break;
            case PIPE_MSG_STREAM_END:
                emit thinkingEnded();
                emit streamEnded();
                break;
            case PIPE_MSG_ERROR:
                emit errorReceived(QString::fromUtf8(msg.data));
                break;
            case PIPE_MSG_TOOL_RESULT:
                emit toolResultReceived(msg.tool_call_id, msg.tool_call_idx, QString::fromUtf8(msg.data));
                break;
            case PIPE_MSG_MODEL_LIST_ITEM:
                emit modelListItemReceived(QString::fromUtf8(msg.data));
                break;
            case PIPE_MSG_MODEL_LIST_END:
                emit modelListEnded();
                break;
            case PIPE_MSG_MODEL_LIST_ERROR:
                emit modelListError(QString::fromUtf8(msg.data));
                break;
        }
    }
}

void PipeHandler::pipeInputCallback(int fd, void *data)
{
    // Static callback wrapper
    PipeHandler *handler = static_cast<PipeHandler*>(data);
    if (handler) {
        handler->handlePipeInput();
    }
}
