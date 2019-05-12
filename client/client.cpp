#include "client.h"

Client::Client(): msgSize_(0)
{
    setupUi(this);
    socket_ = new QTcpSocket(this);
    connect(socket_, SIGNAL(readyRead()), this, SLOT(receivedData()));
    connect(socket_, SIGNAL(connected()), this, SLOT(clientConnect()));
    connect(socket_, SIGNAL(disconnected()), this, SLOT(clientDisconnect()));
    connect(socket_, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
}

void Client::on_connectButton_clicked() {

    msgFrame->append("connecting to the server...");
    connectButton->setEnabled(false);
    socket_->abort();
    socket_->connectToHost(serverIP->text(), serverPORT->value());
}

void Client::on_sendButton_clicked() {
    QByteArray packet;
    QDataStream out(&packet, QIODevice::WriteOnly);
    QString msgToSend = username->text() + ": " + msg->text();
    out << (quint16)0;
    out << msgToSend;
    out.device()->seek(0);
    out << (quint16)(packet.size() - sizeof(quint16));
    socket_->write(packet);
    msg->clear();
    msg->setFocus();
}

void Client::on_msg_returnPressed() {
    on_sendButton_clicked();
}

void Client::receivedData() {
    QDataStream in(socket_);
    if (msgSize_ == 0) {
        if (socket_->bytesAvailable() < (int) sizeof(quint16))
            return;
        in >> msgSize_;
    }
    if(socket_->bytesAvailable() < msgSize_)
        return;
    QString message;
    in >> message;
    msgFrame->append(message);
    msgSize_ = 0;
}

void Client::clientConnect() {
    msgFrame->append("You have been successfully connected !");
    connectButton->setEnabled(true);
}

void Client::clientDisconnect() {
    msgFrame->append("You have been disconnected from the server !");
}

void Client::on_disconnectButton_clicked() {
    qApp->quit(); // Close the client
}

void Client::socketError(QAbstractSocket::SocketError error) {
    switch (error) {
    case QAbstractSocket::HostNotFoundError:
        msgFrame->append("ERROR : Server not found !! Please check your connection settings ans try again ...");
        break;
    case QAbstractSocket::ConnectionRefusedError:
        msgFrame->append("ERROR : Server did not accept the connection !! Please check your connection settings ans try again ...");
        break;
    case QAbstractSocket::RemoteHostClosedError:
        msgFrame->append("ERROR : Connection has been terminated by the server !!");
        break;
    default:
        msgFrame->append("ERROR : " + socket_->errorString());
    }
    connectButton->setEnabled(true);
}
