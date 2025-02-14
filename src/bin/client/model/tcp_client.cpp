#include <QThread>

#include "client/model/session.hpp"
#include "client/model/tcp_client.hpp"
#include "constants.hpp"
#include "message/create_channel.hpp"
#include "message/create_channel_response.hpp"
#include "message/delete_account.hpp"
#include "message/delete_account_response.hpp"
#include "message/header.hpp"
#include "message/list_accounts.hpp"
#include "message/list_accounts_response.hpp"
#include "message/login.hpp"
#include "message/login_response.hpp"
#include "message/register_account.hpp"
#include "message/register_account_response.hpp"
#include "message/send_message.hpp"
#include "message/send_message_response.hpp"
#include "models/message_handler.hpp"

TcpClient::TcpClient(QObject* parent) : QObject(parent) {
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpClient::onErrorOccurred);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

void TcpClient::connectToServer(const QString& host, quint16 port) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Already connected to the server.";
        return;
    }

    if (socket->state() == QAbstractSocket::ConnectingState) {
        qDebug() << "Connection is already in progress.";
        return;
    }

    qDebug() << "Connecting to server at" << host << ":" << port;
    socket->connectToHost(host, port);
}

void TcpClient::register_user(const std::string& username,
                              const std::string& displayName,
                              const std::string& password) {
    RegisterAccountMessage message(username, password, displayName);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
}

void TcpClient::login_user(const std::string& username, const std::string& password) {
    LoginMessage message(username, password);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
}

void TcpClient::search_accounts(const std::string& regex) {
    ListAccountsMessage message(regex);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
}

void TcpClient::delete_account(const std::string& username, const std::string& password) {
    DeleteAccountMessage message(username, password);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
}

void TcpClient::create_channel(const std::string& channelName, const std::vector<UUID>& members) {
    CreateChannelMessage message(channelName, members);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
}

void TcpClient::send_text_message(const UUID& channel_uid,
                                  const UUID& sender_uid,
                                  const std::string& text) {
    SendMessageMessage message(channel_uid, sender_uid, text);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
}

void TcpClient::onReadyRead() {
    Header header;
    if (socket->bytesAvailable() < header.size()) {
        return;
    }
    QByteArray headerData = socket->read(header.size());
    std::vector<uint8_t> vec(headerData.size());
    std::transform(headerData.begin(), headerData.end(), vec.begin(),
                   [](char c) { return static_cast<uint8_t>(c); });
    header.deserialize(vec);
    qDebug() << "Received header: " << header.get_version() << " " << header.get_operation() << " "
             << header.get_packet_length();

    if (header.get_version() != PROTOCOL_VERSION) {
        qDebug() << "Protocol version mismatch";
        return;
    }

    static constexpr uint8_t MAX_RETRIES = 100;
    uint8_t retries = 0;
    while (socket->bytesAvailable() < header.get_packet_length()) {
        if (++retries > MAX_RETRIES) {
            qDebug() << "Timeout waiting for message data";
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
            RegisterAccountResponse response;
            response.deserialize(msg);
            qDebug() << response.to_json().c_str();
            messageHandler.dispatch(socket, response);
            break;
        }
        case Operation::LOGIN: {
            LoginResponse response;
            response.deserialize(msg);
            qDebug() << response.to_json().c_str();
            messageHandler.dispatch(socket, response);
            break;
        }
        case Operation::LIST_ACCOUNTS: {
            ListAccountsResponse response;
            response.deserialize(msg);
            qDebug() << response.to_json().c_str();
            messageHandler.dispatch(socket, response);
            break;
        }
        case Operation::DELETE_ACCOUNT: {
            DeleteAccountResponse response;
            response.deserialize(msg);
            qDebug() << response.to_json().c_str();
            messageHandler.dispatch(socket, response);
            break;
        }
        case Operation::SEND_MESSAGE: {
            SendMessageResponse response;
            response.deserialize(msg);
            qDebug() << response.to_json().c_str();
            messageHandler.dispatch(socket, response);
            break;
        }
        default:
            qDebug() << "Unknown operation";
            break;
    }
}

void TcpClient::disconnectFromServer() {
    socket->disconnectFromHost();
}

QAbstractSocket::SocketState TcpClient::getConnectionStatus() const {
    return socket->state();
}

void TcpClient::onConnected() {
    Session& session = Session::get_instance();
    qDebug() << "Connected to server";
    session.main_window->animatePageTransition(Window::AUTHENTICATION);
}
void TcpClient::onDisconnected() {
    Session& session = Session::get_instance();
    qDebug() << "Disconnected from server";
    session.main_window->animatePageTransition(Window::CONNECTION);
}

void TcpClient::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    qDebug() << "Socket error:" << socket->errorString();
}
