#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QThread>
#include <QObject>
#include <QMutex>
#include <QWaitCondition>
#include <QString>
#include <QList>
#include <QMap>
#include "disasterparty_wrapper.h"

class ThreadWorker : public QObject
{
    Q_OBJECT
public:
    explicit ThreadWorker(QObject *parent = nullptr);
    virtual ~ThreadWorker();

    void startWork();
    void stopWork();
    void wait();
    bool isRunning() const;

    virtual void run() = 0;

signals:
    void finished();
    void error(const QString &err);

protected:
    QThread *m_thread;
    QMutex m_mutex;
    bool m_running;
    void *m_data;
};

class CompletionWorker : public ThreadWorker
{
    Q_OBJECT
public:
    explicit CompletionWorker(dp_context_t *ctx, const dp_request_config_t &config, QObject *parent = nullptr);
    ~CompletionWorker() override;

    void run() override;

signals:
    void tokenReceived(const QString &token);
    void thinkingReceived(const QString &thinkingToken);
    void streamFinished();

private:
    dp_context_t *m_ctx;
    dp_request_config_t m_config;
    static int streamCallback(const char *token, void *user_data, bool is_final_chunk, const char *err);
};

#endif // THREADWORKER_H
