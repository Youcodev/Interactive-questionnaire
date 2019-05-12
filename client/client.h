#ifndef CLIENT_H
#define CLIENT_H

#include <QtGui>
#include <QtNetwork>
#include "ui_client.h"

class Client : public QWidget, private Ui::Client
{
    Q_OBJECT

public:
    Client();
private slots:
    void on_connectButton_clicked(); // Manage client connection to the server
    void on_disconnectButton_clicked(); // Disconnect client and close its window
    void on_sendButton_clicked(); // Send messages to the server
    void on_msg_returnPressed(); // Send messages to the server by pressing enter
    void receivedData(); // Manage packets received by the clients
    void clientConnect(); // Manage connection success
    void clientDisconnect(); // Manage client disconnection from the server
    void socketError(QAbstractSocket::SocketError error); // Manage connection failure

private:
    QTcpSocket* socket_; // Client socket
    quint16 msgSize_; // Message size
};

#endif // CLIENT_H
