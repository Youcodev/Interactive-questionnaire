#include "server.h"

Server::Server(): msgSize_(0) {
    // Setting up the ui design
    setupUi(this);
    // Creating a server window
    server_ = new QTcpServer(this);
    srand((unsigned) time(NULL));
    int randNum = rand()%51 + 10000; // To have a port between 10000 and 10050
    // If the server doesn't start correctly, send an error msg
    if (!server_->listen(QHostAddress::Any, randNum))
         msgFrame->append("The connection to the server has failed : " + server_->errorString());
    else {
        msgFrame->append("The connection to server has been successfully established on PORT " +
                         QString::number(server_->serverPort()));
        connect(server_, SIGNAL(newConnection()), this, SLOT(serverNewConnection()));
    }
}

void Server::sendMsgToAll(const QString& msg) {
    QByteArray packet; // packet to be sent on the network
    QDataStream out(&packet, QIODevice::WriteOnly); // Form (write in) the packet : size (quint 16) and msg (QString)
    out << (quint16) 0; // Reserve a place at the beginning of the packet to include the msg size which is calculated below
    out << msg; // then we add the msg
    out.device()->seek(0); // and we go back to the beginning of the packet
    out << (quint16)(packet.size() - sizeof(quint16)); // we finaly put the right msg size
    //Sending the packet to all clients
    for (int i = 0; i < clients_.size(); ++i)
        clients_[i]->write(packet);
}

void Server::serverNewConnection() {
    QTcpSocket* newClient = server_->nextPendingConnection(); // Get client socket
    clients_ << newClient; // Add the new client to the clients list
    if(!sendButton->isEnabled()) { // Test if the survey is still active (see on_sendButton_clicked() below)
        QByteArray packet; // packet to be sent on the network
        QDataStream out(&packet, QIODevice::WriteOnly); // Form (write in) the packet : size (quint 16) and msg (QString)
        out << (quint16) 0; // Reserve a place at the beginning of the packet to include the msg size (calculated below)
        out << msg->text(); // then we add the msg
        out.device()->seek(0); // and we go back to the beginning of the packet
        out << (quint16)(packet.size() - sizeof(quint16)); // we finaly put the right msg size
        //Sending the packet to the new connected client
        clients_.back()->write(packet);
    }
    connect(newClient, SIGNAL(readyRead()), this, SLOT(receivedData()));
    connect(newClient, SIGNAL(disconnected()), this, SLOT(disconnectClient()));
}

void Server::receivedData() {
    // When a packet is received from a client
    // We first identify the client who sent the msg (looking for the client QTcpSocket)
    QTcpSocket* socket = qobject_cast<QTcpSocket*> (sender());
    // If the client couldn't be identified
    if (socket == 0)
        return;
    //Else (client identified), we try to catch the msg
    QDataStream in(socket);
    // If the msg size isn't yet known
    if (msgSize_ == 0) {
        // We first try to cach it
        // If we didn't receive the entire msg size, we wait for the next packet
        if (socket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> msgSize_;
    }
        // Once we have the msg size, we can now check if we received the entire msg
        if (socket->bytesAvailable() < msgSize_)
            return;
        QString msg;
        in >> msg;
        // If the survey is still active, the server can receive a client answer
        if(!sendButton->isEnabled())
        msgFrame->append(msg);
        // Then we write to a text file (journal.txt) clients answers preceded by their IP address and PORT number
        writeToFile(socket->localAddress(), socket->localPort(), msg);
        // Finaly, we reset the value of msgSize_ in order to correctly receive future packets
        msgSize_ = 0;
}

void Server::disconnectClient() {
    sendMsgToAll("A client has disconnected");
    // We identify this client in order to delete his socket from the connected clients list
    QTcpSocket* socket = qobject_cast<QTcpSocket*> (sender());
    if (socket == 0) // if we couldn't for some reason identify the disconnected client, we return
        return;
    clients_.removeOne(socket);
    socket->deleteLater();
}

void Server::on_exitButton_clicked() {
    qApp->quit(); // Close the server
}

void Server::on_sendButton_clicked() {
    // Send message to clients
       QString msgToSend = profName->text() + ": " + msg->text();
       msgFrame->append(msgToSend);
      sendMsgToAll(msgToSend);
       // Timer activation
       sendButton->setEnabled(false); // Send one question at a time
       // Call timeUp when timer has stopped
       QTimer::singleShot(answerTime->value(), this, SLOT(timeUp()));

}

void Server::timeUp() {
    // Tell clients that survey is finished now
    sendMsgToAll("Time Up !! You cannot submit your answer any more !!");
    sendButton->setEnabled(true);
    msg->clear();
    msg->setFocus();

}

void Server::on_msg_returnPressed() {
    on_sendButton_clicked();
}

void Server::writeToFile(QHostAddress address, quint16 port, QString msg) {
        QFile file("journal.txt"); // Construct a text file named journal
        if (!file.open(QFile::ReadWrite | QFile::Text)) {   //If the file isn't opened
            QMessageBox::warning(this, "Info", "File not opened !!"); // Display a warning message
            return;
        }
        QTextStream inout(&file);
        //Read from the file
        while (!inout.atEnd())
            QString line = inout.readLine();
        //Write to the file after the last line
        inout << address.toString() + " " + QString::number(port) + ": " + msg << endl;
        file.close();
}
