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
#include "message/send_message.hpp"
#include "message/delete_message.hpp"
#include "constants.hpp"

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject *parent) : QObject(parent), socketDescriptor(socketDescriptor)
{
}

void ClientHandler::handleClient()
{
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    authenticated_user = std::nullopt;

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

    if (header.get_version() != PROTOCOL_VERSION)
    {
        return;
    }

    static constexpr uint8_t MAX_RETRIES = 100;
    uint8_t retries = 0;
    while (socket->bytesAvailable() < header.get_packet_length())
    {
        if (++retries > MAX_RETRIES)
        {
            qDebug() << "Too many retries";
            return;
        }
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
        messageHandler.dispatch(socket, registerAccount);
        break;
    }
    case Operation::LOGIN:
    {
        LoginMessage login;
        login.deserialize(msg);
        messageHandler.dispatch(socket, login);
        break;
    }
    case Operation::DELETE_ACCOUNT:
    {

        DeleteAccountMessage deleteAccount;
        deleteAccount.deserialize(msg);
        messageHandler.dispatch(socket, deleteAccount);
        break;
    }
    case Operation::LIST_ACCOUNTS:
    {
        ListAccountsMessage listAccounts;
        listAccounts.deserialize(msg);
        messageHandler.dispatch(socket, listAccounts);
        break;
    }
    case Operation::SEND_MESSAGE:
    {
        SendMessageMessage sendMessage;
        sendMessage.deserialize(msg);
        messageHandler.dispatch(socket, sendMessage);
        break;
    }
    case Operation::DELETE_MESSAGE:
    {
        DeleteMessageMessage deleteMessage;
        deleteMessage.deserialize(msg);
        messageHandler.dispatch(socket, deleteMessage);
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

void ClientHandler::setAuthenticatedUser(const User::SharedPtr &user)
{
    authenticated_user = user;
}