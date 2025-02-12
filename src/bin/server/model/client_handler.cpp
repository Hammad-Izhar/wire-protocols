#include <iostream>
#include <vector>
#include <QThread>

#include "server/model/client_handler.hpp"
#include "models/message_handler.hpp"
#include "message/header.hpp"
#include "message/register_account.hpp"
#include "message/login.hpp"
#include "message/delete_account.hpp"
#include "message/list_accounts.hpp"

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
    if (socket->bytesAvailable() < Header::size())
        return;

    QByteArray headerData = socket->read(Header::size());
    std::vector<uint8_t> vec(headerData.size());
    std::transform(headerData.begin(), headerData.end(), vec.begin(), [](char c)
                   { return static_cast<uint8_t>(c); });
    Header header;
    header.deserialize(vec);
    qDebug() << "Received header: " << header.get_version() << " " << header.get_operation() << " " << header.get_packet_length();

    while (socket->bytesAvailable() < header.get_packet_length())
    {
        QThread::msleep(100);
    }

    QByteArray data = socket->read(header.get_packet_length());
    std::vector<uint8_t> msg(header.get_packet_length());
    std::transform(data.begin(), data.end(), msg.begin(), [](char c)
                   { return static_cast<uint8_t>(c); });

    MessageHandler &messageHandler = MessageHandler::get_instance();
    switch (header.get_operation())
    {
    case Operation::REGISTER_ACCOUNT:
    {
        RegisterAccountMessage registerAccount;
        registerAccount.deserialize(msg);
        messageHandler.dispatch(registerAccount);
        break;
    }
    case Operation::LOGIN:
    {
        LoginMessage login;
        login.deserialize(msg);
        messageHandler.dispatch(login);
        break;
    }
    case Operation::DELETE_ACCOUNT:
    {

        DeleteAccountMessage deleteAccount;
        deleteAccount.deserialize(msg);
        messageHandler.dispatch(deleteAccount);
        break;
    }
    case Operation::LIST_ACCOUNTS:
    {
        ListAccountsMessage listAccounts;
        listAccounts.deserialize(msg);
        messageHandler.dispatch(listAccounts);
        break;
    }

    default:
        qDebug() << "Unknown operation";
        break;
    }
}

void ClientHandler::onDisconnected()
{
    qDebug() << "Client disconnected";
    socket->deleteLater();
    emit finished();
}
