#ifndef COMMUNICATIONHANDLER_H
#define COMMUNICATIONHANDLER_H



#include <QTcpSocket>
#include <QThread>


class CommunicationHandler : public QThread {
    Q_OBJECT

public:
    CommunicationHandler(QObject *parent = nullptr) : QThread(parent), stopFlag(false) {}

    void run() override {
        QTcpSocket socket;
        socket.connectToHost("localhost", 9999);

        if (socket.waitForConnected()) {
            socket.write("0000");
            socket.waitForBytesWritten();

            if (socket.waitForReadyRead()) {
                QByteArray response = socket.readAll();
                if (response == "0000") {
                    emit messageReceived("Code from server: " + response);
                    emit messageReceived("Comms Ok");

                    socket.write("0010");
                    socket.waitForBytesWritten();

                    if (socket.waitForReadyRead()) {
                        QByteArray response2 = socket.readAll();
                        if (response2 == "0010") {
                            emit messageReceived("Code from server: " + response2);
                            emit messageReceived("Starting streaming");

                            while (!stopFlag) {
                                if (socket.waitForReadyRead()) {
                                    QByteArray data = socket.readAll();
                                    emit dataReceived(data);
                                }
                            }
                            socket.write("0001");
                            socket.waitForBytesWritten();
                        } else {
                            emit messageReceived("Sequence broken");
                        }
                    } else {
                        emit messageReceived("No response for 0010");
                    }
                } else {
                    emit messageReceived("Unexpected response for 0000: " + response);
                }
            } else {
                emit messageReceived("No response for 0000");
            }
        } else {
            emit messageReceived("Failed to connect to server");
        }
        socket.close();
    }

    void stop() {
        stopFlag = true;
    }

signals:
    void messageReceived(const QString &message);
    void dataReceived(const QByteArray &data);

private:
    bool stopFlag;
};
#endif // COMMUNICATIONHANDLER_H
