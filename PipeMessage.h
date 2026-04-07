#ifndef PIPE_MESSAGE_H
#define PIPE_MESSAGE_H

#include <QString>
#include <QByteArray>

// Pipe message structure for async communication
struct PipeMessage {
    int type;
    QByteArray data;
    int tool_call_id;
    int tool_call_idx;

    PipeMessage(int t, const QByteArray &d = QByteArray(), int tc_id = -1, int tc_idx = -1) :
        type(t), data(d), tool_call_id(tc_id), tool_call_idx(tc_idx) {}
};

// Pipe message types
enum PipeMessageType {
    PIPE_MSG_TOKEN = 0,
    PIPE_MSG_THINKING = 1,
    PIPE_MSG_STREAM_END = 2,
    PIPE_MSG_ERROR = 3,
    PIPE_MSG_TOOL_RESULT = 4,
    PIPE_MSG_MODEL_LIST_ITEM = 5,
    PIPE_MSG_MODEL_LIST_END = 6,
    PIPE_MSG_MODEL_LIST_ERROR = 7
};

#endif // PIPE_MESSAGE_H
