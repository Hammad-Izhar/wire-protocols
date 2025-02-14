#include <QThread>
#include <vector>

#include "constants.hpp"
#include "message/create_channel.hpp"
#include "message/create_channel_response.hpp"
#include "message/delete_account.hpp"
#include "message/delete_message.hpp"
#include "message/delete_message_response.hpp"
#include "message/header.hpp"
#include "message/list_accounts.hpp"
#include "message/login.hpp"
#include "message/register_account.hpp"
#include "message/send_message.hpp"
#include "message/send_message_response.hpp"
#include "models/message_handler.hpp"
#include "server/model/client_handler.hpp"

ClientHandler::ClientHandler(qintptr socketDescriptor, QObject* parent)
    : QObject(parent), socket_descriptor(socketDescriptor) {}

void ClientHandler::set_authenticated_user(const User::SharedPtr user) {
    if (authenticated_user.has_value()) {
        disconnect(authenticated_user.value().get(), &User::channel_added, this,
                   &ClientHandler::on_channel_added);
        // disconnect(authenticated_user.value().get(), &User::channel_removed, this,
        //            &on_channel_removed);
        disconnect(authenticated_user.value().get(), &User::message_received, this,
                   &ClientHandler::on_message_received);
        disconnect(authenticated_user.value().get(), &User::message_deleted, this,
                   &ClientHandler::on_message_deleted);
    }

    authenticated_user = user;
    connect(user.get(), &User::channel_added, this, &ClientHandler::on_channel_added);
    // connect(user.get(), &User::channel_removed, this, &on_channel_removed);
    connect(user.get(), &User::message_received, this, &ClientHandler::on_message_received);
    connect(user.get(), &User::message_deleted, this, &ClientHandler::on_message_deleted);
}

void ClientHandler::handle_client() {
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socket_descriptor);
    authenticated_user = std::nullopt;

    connect(&MessageHandler::get_instance(), &MessageHandler::write_data, this,
            &ClientHandler::on_write_data);
    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::on_read_data);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::on_disconnected);

    qDebug() << "New Client: " << socket->peerAddress() << ":" << socket->peerPort();
}

void ClientHandler::on_write_data(std::vector<uint8_t> data) {
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
}

void ClientHandler::on_read_data() {
    Header header;
    if (socket->bytesAvailable() < header.size())
        return;

    QByteArray headerData = socket->read(header.size());
    std::vector<uint8_t> vec(headerData.size());
    std::transform(headerData.begin(), headerData.end(), vec.begin(),
                   [](char c) { return static_cast<uint8_t>(c); });
    header.deserialize(vec);
    qDebug() << "Received header: " << header.get_version() << " " << header.get_operation() << " "
             << header.get_packet_length();

    if (header.get_version() != PROTOCOL_VERSION) {
        return;
    }

    static constexpr uint8_t MAX_RETRIES = 100;
    uint8_t retries = 0;
    while (socket->bytesAvailable() < header.get_packet_length()) {
        if (++retries > MAX_RETRIES) {
            qDebug() << "Too many retries";
            return;
        }
        QThread::msleep(100);
    }

    QByteArray data = socket->read(header.get_packet_length());
    std::vector<uint8_t> msg(header.get_packet_length());
    std::transform(data.begin(), data.end(), msg.begin(),
                   [](char c) { return static_cast<uint8_t>(c); });

    MessageHandler& messageHandler = MessageHandler::get_instance();
    switch (header.get_operation()) {
        case Operation::REGISTER_ACCOUNT: {
            RegisterAccountMessage registerAccount;
            registerAccount.deserialize(msg);
            qDebug() << registerAccount.to_json().c_str();
            messageHandler.dispatch(socket, registerAccount);
            break;
        }
        case Operation::LOGIN: {
            LoginMessage login;
            login.deserialize(msg);
            qDebug() << login.to_json().c_str();
            messageHandler.dispatch(socket, login);
            break;
        }
        case Operation::DELETE_ACCOUNT: {
            DeleteAccountMessage deleteAccount;
            deleteAccount.deserialize(msg);
            qDebug() << deleteAccount.to_json().c_str();
            messageHandler.dispatch(socket, deleteAccount);
            break;
        }
        case Operation::LIST_ACCOUNTS: {
            ListAccountsMessage listAccounts;
            listAccounts.deserialize(msg);
            qDebug() << listAccounts.to_json().c_str();
            messageHandler.dispatch(socket, listAccounts);
            break;
        }
        case Operation::CREATE_CHANNEL: {
            CreateChannelMessage createChannel;
            createChannel.deserialize(msg);
            qDebug() << createChannel.to_json().c_str();
            messageHandler.dispatch(socket, createChannel);
            break;
        }
        case Operation::SEND_MESSAGE: {
            SendMessageMessage sendMessage;
            sendMessage.deserialize(msg);
            qDebug() << sendMessage.to_json().c_str();
            messageHandler.dispatch(socket, sendMessage);
            break;
        }
        case Operation::DELETE_MESSAGE: {
            DeleteMessageMessage deleteMessage;
            deleteMessage.deserialize(msg);
            qDebug() << deleteMessage.to_json().c_str();
            messageHandler.dispatch(socket, deleteMessage);
            break;
        }
        default:
            qDebug() << "Unknown operation";
            break;
    }
}

void ClientHandler::on_disconnected() {
    qDebug() << "Client disconnected";
    socket->deleteLater();
    emit finished();
}

void ClientHandler::on_message_received(std::variant<Message::SharedPtr, std::string> message) {
    SendMessageResponse response(message);
    qDebug() << "Message received" << response.to_json().c_str();
    std::vector<uint8_t> buf;
    response.serialize_msg(buf);
    emit MessageHandler::get_instance().write_data(buf);
}

void ClientHandler::on_message_deleted(std::variant<Message::SharedPtr, std::string> message) {
    DeleteMessageResponse response(message);
    qDebug() << "Message deleted" << response.to_json().c_str();
    std::vector<uint8_t> buf;
    response.serialize_msg(buf);
    emit MessageHandler::get_instance().write_data(buf);
}

void ClientHandler::on_channel_added(std::variant<Channel::SharedPtr, std::string> channel) {
    CreateChannelResponse response(channel);
    qDebug() << "Channel added" << response.to_json().c_str();
    std::vector<uint8_t> buf;
    response.serialize_msg(buf);
    emit MessageHandler::get_instance().write_data(buf);
}

// void ClientHandler::on_channel_removed(std::variant<Channel::SharedPtr, std::string> channel) {
//     DeleteChannelResponse response(channel);
//     std::vector<uint8_t> buf;
//     response.serialize_msg(buf);
//     emit MessageHandler::get_instance().write_data(buf);
// }
