#include <iostream>

#include "server/model/client_handler.hpp"

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent) : QObject(parent), socketDescriptor(socketDescriptor)
{
}

void ClientHandler::handleClient()
{
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);

    qDebug() << "New Client: " << socket->peerAddress() << ":" << socket->peerPort();
}

void ClientHandler::onReadyRead()
{
    while (socket->bytesAvailable() > 0)
    {
        QByteArray data = socket->readAll();
        qDebug() << "Received data: " << data.toStdString().c_str();
    }
}

void ClientHandler::onDisconnected()
{
    qDebug() << "Client disconnected";
    socket->deleteLater();
    emit finished();
}
