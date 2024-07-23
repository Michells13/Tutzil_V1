// worker.h
#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QProcess>
#include <QMutex>
#include <QMutexLocker>
#include <QWaitCondition>
#include <atomic>
class Worker : public QObject {
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

signals:
    void updateTextBrowser(const QString &text);
    void finished();

public slots:
    void doWork();
    void stopComms();


private slots:
    void handleProcessOutput();
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);


private:
    QTcpSocket *socket;
    QProcess *pythonProcess;
    bool comms;
    std::atomic<bool> m_stop;  // Use atomic for thread-safe boolean flag m_stop;
    QMutex m_mutex;
    QWaitCondition condition;
};

#endif // WORKER_H
