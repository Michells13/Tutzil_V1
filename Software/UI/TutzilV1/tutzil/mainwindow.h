#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QProcess>
#include <worker.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void handleUpdateTextBrowser(const QString &text);
    void handleWorkerFinished();





signals:
    void normalStop(); // Signal to change state





private:
    Ui::MainWindow *ui;
    bool processActive; // Variable to control the process
    bool EmergencyStop;  // Add this line to declare EmergencyStop
    bool NormalStop;
    bool comms_UI;
    QProcess *m_process; // Declaración de m_process como variable miembro
    QTcpSocket *tcpSocket; // Socket for communication
    QThread *workerThread;
    Worker *worker;
    QMutex mmutex;








};
#endif // MAINWINDOW_H
