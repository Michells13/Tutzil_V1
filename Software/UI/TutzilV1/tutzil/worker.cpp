// worker.cpp
#include "worker.h"
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include <atomic>

Worker::Worker(QObject *parent) : QObject(parent), socket(nullptr), pythonProcess(nullptr), comms(false),m_stop(false) {}

Worker::~Worker() {
    if (socket) {
        socket->close();
        delete socket;
    }
    if (pythonProcess) {
        pythonProcess->close();
        delete pythonProcess;
    }
}

void Worker::doWork() {
    int i=0;
    pythonProcess = new QProcess();
    connect(pythonProcess, &QProcess::readyReadStandardOutput, this, &Worker::handleProcessOutput);
    connect(pythonProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(handleProcessFinished(int, QProcess::ExitStatus)));

    QString virtualenvPath = "C:/Users/MICHE/anaconda3/envs/pyT/python.exe";
    QString scriptPath = "C:/Users/MICHE/Desktop/sntil/Software/Vision/YV8_detect_v4_.py";

    pythonProcess->start(virtualenvPath, QStringList() << scriptPath);
    pythonProcess->waitForStarted(2000);

    emit updateTextBrowser("Starting vision module... \n");
    socket = new QTcpSocket();
    socket->connectToHost("localhost", 9999);

    if (socket->waitForConnected()) {
        socket->write("0000");
        socket->waitForBytesWritten();

        if (socket->waitForReadyRead()) {
            QByteArray response = socket->readAll();
            if (response == "0000") {
                emit updateTextBrowser("Code from server: " + response + "\nComms Ok \n");

                socket->write("0010");
                socket->waitForBytesWritten();

                if (socket->waitForReadyRead()) {
                    QByteArray response2 = socket->readAll();
                    if (response2 == "0010") {
                        emit updateTextBrowser("Code from server: " + response2 + "\nStarting streaming \n");


                        while(i<10 ){

                            socket->write("0100");
                            socket->waitForBytesWritten();
                            QString str = QString::number(i);
                            QString str1 = QString::number(comms);

                                emit updateTextBrowser("Reading image"+str+" \n");
                                emit updateTextBrowser("Bool state"+str1+" \n");
                                i=i+1;

                                QMutexLocker locker(&m_mutex);
                                if (m_stop) {
                                    emit finished();
                                }
                             QThread::msleep(1000);  // Add a short sleep to avoid tight looping


                        }

                        socket->write("0001");

                        socket->waitForBytesWritten();
                        emit updateTextBrowser("Closing comms \n");




                    } else {
                        emit updateTextBrowser("Sequence broken, responce expected: 0010 \n");
                        emit updateTextBrowser("Responce from server was: "+ response + "\n");


                    }
                } else {
                    emit updateTextBrowser("No response for 0010 \n");
                }
            } else {
                emit updateTextBrowser("Unexpected response for 0000: " + response + "\n");
            }
        } else {
            emit updateTextBrowser("No response for 0000 \n");
        }
    } else {
        emit updateTextBrowser("Failed to connect to server.... \n");
    }

    emit finished();
}

void Worker::stopComms() {
    QMutexLocker locker(&m_mutex);
    m_stop = true;
}

void Worker::handleProcessOutput()
{

}

void Worker::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{

}
