#include "ThreadWorker.h"

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
    
    // Connect signals
    connect(m_thread, &QThread::started, this, &ThreadWorker::run);
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
