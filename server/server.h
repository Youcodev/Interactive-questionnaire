#ifndef SERVER_H
#define SERVER_H

#include <QtGui>
#include <QtNetwork>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <cstdlib>
#include <ctime>
#include "ui_server.h"

class Server : public QDialog, private Ui::server {
    Q_OBJECT
public:
    Server();
    void sendMsgToAll(const QString& msg); // Send a message to all clients
    private slots:
    void serverNewConnection(); // Manage client connection to the server
    void receivedData(); // Manage packets received by the server
    void disconnectClient(); // Manage client disconnection
    void timeUp(); // Manage timeout action
    void writeToFile(QHostAddress address, quint16 port, QString msg); // Save clients answers in a file
    void on_exitButton_clicked(); // Close the server
    void on_sendButton_clicked(); // Send messages to clients
    void on_msg_returnPressed(); // Send messages to clients by pressing enter
private:
    QTcpServer* server_; // TCP server
    QList<QTcpSocket*> clients_; // Liste of clients
    quint16 msgSize_; // Message size
};

#endif

