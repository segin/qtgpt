#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class ThreadWorker : public QObject
{
public:
    explicit ThreadWorker(QObject *parent = nullptr);
    virtual ~ThreadWorker();

    // Thread management
    void startWork();
    void stopWork();
    void wait();
    bool isRunning() const;

    // Base class for thread workers
    virtual void run() = 0;

protected:
    QThread *m_thread;
    QMutex m_mutex;
    bool m_running;

    // Worker state
    void *m_data;
};

#endif // THREADWORKER_H
