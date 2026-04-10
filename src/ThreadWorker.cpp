#include "ThreadWorker.h"
#include <QDebug>

ThreadWorker::ThreadWorker(QObject *parent)
    : QObject(parent)
{
    m_thread = nullptr;
    m_running = false;
    m_data = nullptr;
}

ThreadWorker::~ThreadWorker()
{
    if (m_thread && m_thread->isRunning()) {
        m_thread->quit();
        m_thread->wait();
    }
    // m_thread deletes itself via deleteLater
}

void ThreadWorker::startWork()
{
    m_thread = new QThread();
    m_data = nullptr;
    m_running = true;
    
    this->setParent(nullptr);
    this->moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, &ThreadWorker::run);
    connect(this, &ThreadWorker::finished, m_thread, &QThread::quit);
    connect(m_thread, &QThread::finished, m_thread, &QThread::deleteLater);
    connect(m_thread, &QThread::finished, this, &QObject::deleteLater);

    m_thread->start();
}

void ThreadWorker::stopWork()
{
    m_running = false;
    if (m_thread && m_thread->isRunning()) {
        m_thread->quit();
    }
}

void ThreadWorker::wait()
{
    if (m_thread && m_thread->isRunning()) {
        m_thread->wait();
    }
}

bool ThreadWorker::isRunning() const
{
    return m_running;
}

CompletionWorker::CompletionWorker(dp_context_t *ctx, const dp_request_config_t &config, QObject *parent)
    : ThreadWorker(parent), m_ctx(ctx), m_config(config)
{
}

CompletionWorker::~CompletionWorker()
{
    // Need to properly free the messages in config.
    if (m_config.messages) {
        dp_free_messages((dp_message_t*)m_config.messages, m_config.num_messages);
    }
    if (m_config.system_prompt) {
        free((void*)m_config.system_prompt);
    }
    if (m_config.model) {
        free((void*)m_config.model);
    }
}

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

int CompletionWorker::streamCallback(const dp_stream_event_t *event, void *user_data, const char *err)
{
    CompletionWorker *worker = static_cast<CompletionWorker*>(user_data);
    if (!worker->isRunning()) {
        return 1; // Abort
    }
    if (err) {
        emit worker->error(QString::fromUtf8(err));
        return 0;
    }
    
    if (event) {
        if (event->event_type == DP_EVENT_MESSAGE_STOP) {
            emit worker->streamFinished();
        } else if (event->event_type == DP_EVENT_CONTENT_BLOCK_DELTA || event->event_type == DP_EVENT_THINKING_DELTA) {
            if (event->raw_json_data) {
                QJsonDocument doc = QJsonDocument::fromJson(QByteArray(event->raw_json_data));
                if (doc.isNull() || doc.isEmpty()) {
                    // Fallback for simple strings
                    if (event->event_type == DP_EVENT_CONTENT_BLOCK_DELTA) {
                        emit worker->tokenReceived(QString::fromUtf8(event->raw_json_data));
                    } else if (event->event_type == DP_EVENT_THINKING_DELTA) {
                        emit worker->thinkingReceived(QString::fromUtf8(event->raw_json_data));
                    }
                } else {
                    QJsonObject obj = doc.object();
                    // Anthropic
                    if (obj.contains("delta")) {
                        QJsonObject delta = obj["delta"].toObject();
                        if (delta["type"].toString() == "text_delta") {
                            QString text = delta["text"].toString();
                            if (!text.isEmpty()) emit worker->tokenReceived(text);
                        } else if (delta["type"].toString() == "thinking_delta") {
                            QString thinking = delta["thinking"].toString();
                            if (!thinking.isEmpty()) emit worker->thinkingReceived(thinking);
                        }
                    } 
                    // OpenAI
                    else if (obj.contains("choices")) {
                        QJsonArray choices = obj["choices"].toArray();
                        if (!choices.isEmpty()) {
                            QJsonObject delta = choices[0].toObject()["delta"].toObject();
                            if (delta.contains("content")) {
                                QString text = delta["content"].toString();
                                if (!text.isEmpty()) emit worker->tokenReceived(text);
                            }
                            if (delta.contains("reasoning_content")) {
                                QString thinking = delta["reasoning_content"].toString();
                                if (!thinking.isEmpty()) emit worker->thinkingReceived(thinking);
                            }
                        }
                    }
                    // Gemini (if structured this way)
                    else if (obj.contains("candidates")) {
                        QJsonArray candidates = obj["candidates"].toArray();
                        if (!candidates.isEmpty()) {
                            QJsonObject content = candidates[0].toObject()["content"].toObject();
                            QJsonArray parts = content["parts"].toArray();
                            for (const QJsonValue &part : parts) {
                                QJsonObject pObj = part.toObject();
                                if (pObj.contains("text")) {
                                    QString text = pObj["text"].toString();
                                    if (!text.isEmpty()) emit worker->tokenReceived(text);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

void CompletionWorker::run()
{
    dp_response_t response;
    memset(&response, 0, sizeof(dp_response_t));
    int ret = dp_perform_detailed_streaming_completion(m_ctx, &m_config, streamCallback, this, &response);
    
    if (ret != 0) {
        QString errStr;
        if (response.error_message) {
            errStr = QString("Error (HTTP %1): %2").arg(response.http_status_code).arg(response.error_message);
        } else {
            errStr = "LLM Request Failed.";
        }
        emit error(errStr);
        emit streamFinished();
    }
    
    dp_free_response_content(&response);
    m_running = false;
    emit finished();
}
