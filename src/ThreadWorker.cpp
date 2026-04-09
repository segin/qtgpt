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
    delete m_thread;
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

int CompletionWorker::streamCallback(const char *token, void *user_data, bool is_final_chunk, const char *err)
{
    CompletionWorker *worker = static_cast<CompletionWorker*>(user_data);
    if (!worker->isRunning()) {
        return 1; // Abort
    }
    if (err) {
        emit worker->error(QString::fromUtf8(err));
        return 0;
    }
    if (token) {
        emit worker->tokenReceived(QString::fromUtf8(token));
    }
    if (is_final_chunk) {
        emit worker->streamFinished();
    }
    return 0;
}

void CompletionWorker::run()
{
    dp_response_t response = {0};
    int ret = dp_perform_streaming_completion(m_ctx, &m_config, streamCallback, this, &response);
    
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
