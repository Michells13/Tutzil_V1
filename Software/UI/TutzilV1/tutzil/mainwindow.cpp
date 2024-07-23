// mainwindow.cpp
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "worker.h" // Add this line to include the Worker class
#include <QThread>  // Include the QThread class
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_process(nullptr)
    , tcpSocket(nullptr)
    , workerThread(nullptr)
    , worker(nullptr)
    , EmergencyStop(false)  // Initialize EmergencyStop
    , NormalStop(false)
    , comms_UI(false)

{       //Start comms thread and wait for python script to communicate with thread
    Worker *worker = new Worker();// in the future if multiple cameras are connected we only have to increase the number of workers
    workerThread = new QThread();
    worker->moveToThread(workerThread);

    connect(workerThread, &QThread::started, worker, &Worker::doWork);
    connect(this, &MainWindow::normalStop, worker, &Worker::stopComms);
    connect(worker, &Worker::updateTextBrowser, this, &MainWindow::handleUpdateTextBrowser);
    connect(worker, &Worker::finished, this, &MainWindow::handleWorkerFinished);
    connect(worker, &Worker::finished, workerThread, &QThread::quit);
    connect(worker, &Worker::finished, worker, &Worker::deleteLater);
    connect(workerThread, &QThread::finished, workerThread, &QThread::deleteLater);
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
    if (workerThread) {
        workerThread->quit();
        workerThread->wait();
        delete workerThread;
    }
    if (m_process) {
        m_process->close();
        delete m_process;
        m_process = nullptr;
    }
}

void MainWindow::on_pushButton_clicked() {
    NormalStop = false;
    ui->textBrowser->insertPlainText("Starting vision module... \n");
    // Run the python script
    if (!m_process) {
        m_process = new QProcess();
        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(visionProcessFinished(int, QProcess::ExitStatus)));
    }
    ui->textBrowser->insertPlainText("Starting communication... \n");


    workerThread->start();
}

void MainWindow::on_pushButton_2_clicked() {
    if (worker) {
        QMetaObject::invokeMethod(worker, "stop", Qt::QueuedConnection);
        worker->finished();
    }
    if (m_process) {
        m_process->close();
        delete m_process;
        m_process = nullptr;
    }
    if (workerThread) {
        workerThread->quit();
        workerThread->wait();
        delete workerThread;
    }

    worker->stopComms();


    ui->textBrowser->insertPlainText("_____________________________ \n");
    ui->textBrowser->insertPlainText("Vision module ended.... \n");
    ui->textBrowser->insertPlainText("Communication thread killed.... \n");





}

/*
void MainWindow::visionProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{


    // Manejar el evento de finalización del proceso
    if (exitStatus == QProcess::NormalExit) {
        ui->textBrowser->insertPlainText(" Vision Proces, exit code: \n" + exitCode);

    } else {
        ui->textBrowser->insertPlainText( " Vision Process, weird exit,exit code: \n"+exitCode);


    }
}

void MainWindow::visionProcessOutput()
{
    // Este slot se activa cuando hay datos disponibles en la salida estándar del proceso
    QByteArray processOutput = m_process->readAllStandardOutput(); // Lee los datos de la salida estándar del proceso
    //qDebug() << "Salida del script de Python:" << processOutput;
    ui->textBrowser->insertPlainText("Vision module output: "+processOutput+ "\n");
}
*/
void MainWindow::on_pushButton_3_clicked() {
    // Implement additional button functionality if needed
}

void MainWindow::handleUpdateTextBrowser(const QString &text) {
    ui->textBrowser->insertPlainText(text);
}

void MainWindow::handleWorkerFinished() {
    ui->textBrowser->insertPlainText("Worker finished its task.\n");
    workerThread = nullptr;
}







