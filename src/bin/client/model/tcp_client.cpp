#include "client/model/tcp_client.hpp"
#include "client/model/session.hpp"

TcpClient::TcpClient(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpClient::onErrorOccurred);
}

void TcpClient::connectToServer(const QString &host, quint16 port)
{
    if (socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Already connected to the server.";
        return;
    }

    if (socket->state() == QAbstractSocket::ConnectingState)
    {
        qDebug() << "Connection is already in progress.";
        return;
    }

    qDebug() << "Connecting to server at" << host << ":" << port;
    socket->connectToHost(host, port);
}

void TcpClient::disconnectFromServer()
{
    socket->disconnectFromHost();
}

QAbstractSocket::SocketState TcpClient::getConnectionStatus() const
{
    return socket->state();
}

void TcpClient::onConnected()
{
    Session &session = Session::getInstance();
    qDebug() << "Connected to server";
    session.main_window->animatePageTransition(Window::AUTHENTICATION);
}
void TcpClient::onDisconnected()
{
    Session &session = Session::getInstance();
    qDebug() << "Disconnected from server";
    session.main_window->animatePageTransition(Window::CONNECTION);
}

void TcpClient::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error:" << socket->errorString();
}
