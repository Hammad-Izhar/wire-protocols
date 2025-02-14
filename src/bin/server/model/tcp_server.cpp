#include "server/model/tcp_server.hpp"
#include "server/model/client_handler.hpp"

#include <QThread>

TcpServer::TcpServer(QObject* parent) : QTcpServer(parent) {}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
    qDebug() << "New client connected?";
    QThread* thread = new QThread();
    ClientHandler* handler = new ClientHandler(socketDescriptor);

    handler->moveToThread(thread);

    connect(thread, &QThread::started, handler, &ClientHandler::handle_client);
    connect(handler, &ClientHandler::finished, thread, &QThread::quit);
    connect(handler, &ClientHandler::finished, handler, &ClientHandler::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
}
