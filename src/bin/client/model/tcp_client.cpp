#include <QThread>

#include <qglobal.h>
#include "client/model/tcp_client.hpp"

#include "client/model/session.hpp"
#ifdef PROTOCOL_RPC
#include <google/protobuf/empty.pb.h>
#include <grpcpp/grpcpp.h>
#include "socketout.pb.h"
#else
#include "constants.hpp"
#include "message/create_channel.hpp"
#include "message/create_channel_response.hpp"
#include "message/delete_account.hpp"
#include "message/delete_account_response.hpp"
#include "message/delete_message.hpp"
#include "message/delete_message_response.hpp"
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
#endif

TcpClient::TcpClient(QObject* parent) : QObject(parent) {
#ifndef PROTOCOL_RPC
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &TcpClient::onDisconnected);
    connect(socket, &QTcpSocket::errorOccurred, this, &TcpClient::onErrorOccurred);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
#endif
}

void TcpClient::connectToServer(const QString& host, quint16 port) {
#ifdef PROTOCOL_RPC
    qDebug() << "Connecting to server at" << host << ":" << port;
    auto channel = grpc::CreateChannel(host.toStdString() + ":" + std::to_string(port),
                                       grpc::InsecureChannelCredentials());
    stub = socketout::SocketOut::NewStub(channel);

    onConnected();
#else
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
#endif
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

void TcpClient::register_user(const std::string& username,
                              const std::string& displayName,
                              const std::string& password) {
    qDebug() << "Registring a new user: " << QString::fromStdString(username) << " "
             << QString::fromStdString(displayName);
#ifdef PROTOCOL_RPC
    socketout::RegisterRequest message;
    message.set_username(username);
    message.set_display_name(displayName);
    message.set_password(password);

    google::protobuf::Empty response;
    grpc::ClientContext context;
    grpc::Status status = stub->register_user(&context, message, &response);
    if (status.ok()) {
        emit registrationSuccess();
    } else {
        emit registrationFailure(QString::fromStdString(status.error_message()));
    }
#else
    RegisterAccountMessage message(username, password, displayName);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
#endif
}

void TcpClient::login_user(const std::string& username, const std::string& password) {
    qDebug() << "Logging in user: " << QString::fromStdString(username);
#ifdef PROTOCOL_RPC
    socketout::LoginRequest request;
    request.set_username(username);
    request.set_password(password);

    socketout::LoginResponse response;
    grpc::ClientContext context;

    auto status = stub->login_user(&context, request, &response);
    if (status.ok()) {
        User::SharedPtr usr = std::make_shared<User>(
            response.user().username(), response.user().display_name(),
            UUID::from_string(response.user().uuid()), response.user().profile_picture());
        Session::get_instance().authenticated_user = usr;

        qDebug() << "Authenticated user: " << QString::fromStdString(usr->get_username());
        qDebug() << "Authenticated display name: "
                 << QString::fromStdString(usr->get_display_name());
        qDebug() << "Authenticated profile pic: " << QString::fromStdString(usr->get_profile_pic());

        emit loginSuccess();
    } else {
        emit loginFailure(QString::fromStdString(status.error_message()));
        return;
    }

    auto channel_context = std::make_unique<grpc::ClientContext>();
    auto channel_reader = stub->subscribe_channels(channel_context.get(), request);

    auto message_context = std::make_unique<grpc::ClientContext>();
    auto message_reader = stub->subscribe_messages(message_context.get(), request);

    int random_key = std::rand() % 1000;
    is_connected[random_key].store(true);
    current_thread_key = random_key;

    std::thread t_channel([this, curr_key = random_key,
                           channel_context = std::move(channel_context),
                           channel_reader = std::move(channel_reader)]() mutable {
        qDebug() << "Subscribing to channels";

        socketout::ChannelResponse response;
        while (channel_reader->Read(&response)) {
            if (!is_connected[curr_key].load()) {
                qDebug() << "User manually disconnected";
                // remove the current key
                is_connected.erase(curr_key);
                break;
            }

            Session& session = Session::get_instance();

            session.authenticated_user.value()->add_channel(
                UUID::from_string(response.channel().uuid()));
            std::vector<UUID> members;
            for (const auto& member : response.channel().user_ids()) {
                members.push_back(UUID::from_string(member));
            }

            Channel::SharedPtr channel = std::make_shared<Channel>(
                UUID::from_string(response.channel().uuid()), response.channel().channel_name(),
                members, std::vector<uint64_t>{});

            qDebug() << "Received channel on login: " << channel->to_json().c_str();

            session.add_channel(channel);
            emit createChannelSuccess(channel);
        }
        grpc::Status status = channel_reader->Finish();
        qDebug() << "Channel subscription finished with status: " << status.error_message().c_str();
    });

    std::thread t_message([this, curr_key = random_key,
                           message_context = std::move(message_context),
                           message_reader = std::move(message_reader)]() mutable {
        qDebug() << "Subscribing to messages";

        socketout::MessageResponse response;
        while (message_reader->Read(&response)) {
            if (!is_connected[curr_key].load()) {
                qDebug() << "User manually disconnected";
                is_connected.erase(curr_key);
                break;
            }

            Session& session = Session::get_instance();
            if (response.type() == socketout::Operation::CREATE) {
                std::vector<UUID> read_by;
                for (const auto& reader : response.msg().read_by()) {
                    read_by.push_back(UUID::from_string(reader));
                }

                Message::SharedPtr message = std::make_shared<Message>(
                    UUID::from_string(response.msg().sender_id()),
                    UUID::from_string(response.msg().channel_id()), response.msg().text(),
                    response.msg().snowflake(), response.msg().created_at(),
                    response.msg().modified_at(), read_by);

                qDebug() << "Received message on login: "
                         << QString::fromStdString(message->to_json());

                session.add_message(message);
                emit sendMessageSuccess(message);
            } else if (response.type() == socketout::Operation::DELETE) {
                Message::SharedPtr message = std::make_shared<Message>(
                    UUID::from_string(response.msg().sender_id()),
                    UUID::from_string(response.msg().channel_id()), response.msg().text(),
                    response.msg().snowflake(), response.msg().created_at(),
                    response.msg().modified_at(), std::vector<UUID>());

                session.remove_message(message);
                emit deleteMessageSuccess(message);
            }
        }

        grpc::Status status = message_reader->Finish();
        qDebug() << "Message subscription finished with status: " << status.error_code() << " "
                 << status.error_message().c_str();
    });

    t_channel.detach();
    t_message.detach();

#else
    LoginMessage message(username, password);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
#endif
}

void TcpClient::search_accounts(const std::string& regex) {
    qDebug() << "Searching for accounts matching regex: " << QString::fromStdString(regex);
#ifdef PROTOCOL_RPC
    socketout::ListAccountsRequest message;
    message.set_regex(regex);

    socketout::ListAccountResponse response;
    grpc::ClientContext context;
    grpc::Status status = stub->list_accounts(&context, message, &response);

    if (status.ok()) {
        std::vector<User::SharedPtr> accounts;
        for (const auto& account : response.users()) {
            accounts.push_back(std::make_shared<User>(account.username(), account.display_name(),
                                                      UUID::from_string(account.uuid()),
                                                      account.profile_picture()));
        }
        emit searchSuccess(accounts);
    } else {
        emit searchFailure(QString::fromStdString(status.error_message()));
    }
#else
    ListAccountsMessage message(regex);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
#endif
}

void TcpClient::delete_account(const std::string& username, const std::string& password) {
    qDebug() << "Deleting account: " << QString::fromStdString(username);
#ifdef PROTOCOL_RPC
    socketout::DeleteAccountRequest message;
    message.set_username(username);
    message.set_password(password);

    socketout::DeleteAccountResponse response;
    grpc::ClientContext context;

    grpc::Status status = stub->delete_account(&context, message, &response);
    if (status.ok()) {
        Session& session = Session::get_instance();
        session.reset();
        session.main_window->animatePageTransition(Window::AUTHENTICATION);
        emit deleteAccountSuccess();
    } else {
        emit deleteAccountFailure(QString::fromStdString(status.error_message()));
    }
#else
    DeleteAccountMessage message(username, password);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
#endif
}

void TcpClient::create_channel(const std::string& channelName, const std::vector<UUID>& members) {
    qDebug() << "Creating channel: " << QString::fromStdString(channelName);
    if (members.empty()) {
        emit createChannelFailure("Channel must have at least one member");
        return;
    }
#ifdef PROTOCOL_RPC
    socketout::CreateChannelRequest message;
    message.set_channel_name(channelName);
    for (int i = 0; i < members.size(); i++) {
        message.add_members(members[i].to_string());
    }

    google::protobuf::Empty response;
    grpc::ClientContext context;
    grpc::Status status = stub->create_channel(&context, message, &response);

    if (!status.ok()) {
        emit createChannelFailure(QString::fromStdString(status.error_message()));
    }
#else
    CreateChannelMessage message(channelName, members);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
#endif
}

void TcpClient::send_text_message(const UUID& channel_uid,
                                  const UUID& sender_uid,
                                  const std::string& text) {
    qDebug() << "Sending message to channel: " << QString::fromStdString(channel_uid.to_string())
             << " from " << QString::fromStdString(sender_uid.to_string())
             << " with text: " << QString::fromStdString(text);
#ifdef PROTOCOL_RPC
    socketout::SendMessageRequest message;
    message.set_channel_id(channel_uid.to_string());
    message.set_sender_id(sender_uid.to_string());
    message.set_data(text);

    google::protobuf::Empty response;
    grpc::ClientContext context;
    grpc::Status status = stub->send_message(&context, message, &response);

    if (!status.ok()) {
        emit sendMessageFailure(QString::fromStdString(status.error_message()));
    }
#else
    SendMessageMessage message(channel_uid, sender_uid, text);
    std::vector<uint8_t> data;
    message.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
#endif
}

void TcpClient::delete_message(Message::SharedPtr message) {
    qDebug() << "Deleting message with snowflake: " << message->get_snowflake()
             << " from channel: " << QString::fromStdString(message->get_channel_id().to_string());

#ifdef PROTOCOL_RPC
    socketout::DeleteMessageRequest msg;
    msg.set_channel_id(message->get_channel_id().to_string());
    msg.set_message_id(message->get_snowflake());

    google::protobuf::Empty response;
    grpc::ClientContext context;
    grpc::Status status = stub->delete_message(&context, msg, &response);

    if (status.ok()) {
        emit deleteMessageSuccess(message);
    } else {
        emit deleteMessageFailure(QString::fromStdString(status.error_message()));
    }
#else
    Session& session = Session::get_instance();
    DeleteMessageMessage msg(message->get_channel_id(), message->get_snowflake());
    std::vector<uint8_t> data;
    msg.serialize_msg(data);
    socket->write(reinterpret_cast<const char*>(data.data()), data.size());
    socket->flush();
#endif
}

void TcpClient::disconnectFromServer() {
#ifdef PROTOCOL_RPC
    stub = nullptr;
    is_connected[current_thread_key].store(false);
    onDisconnected();
#else
    socket->disconnectFromHost();
#endif
}

#ifndef PROTOCOL_RPC
void TcpClient::onReadyRead() {
    while (true) {
        Header header;
        if (socket->bytesAvailable() < header.size()) {
            return;
        }
        QByteArray headerData = socket->read(header.size());
        std::vector<uint8_t> vec(headerData.size());
        std::transform(headerData.begin(), headerData.end(), vec.begin(),
                       [](char c) { return static_cast<uint8_t>(c); });
        header.deserialize(vec);
        qDebug() << "Received header: " << header.get_version() << " " << header.get_operation()
                 << " " << header.get_packet_length();

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
            case Operation::DELETE_MESSAGE: {
                DeleteMessageResponse response;
                response.deserialize(msg);
                qDebug() << response.to_json().c_str();
                messageHandler.dispatch(socket, response);
                break;
            }
            case Operation::CREATE_CHANNEL: {
                CreateChannelResponse response;
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
}

QAbstractSocket::SocketState TcpClient::getConnectionStatus() const {
    return socket->state();
}

void TcpClient::onErrorOccurred(QAbstractSocket::SocketError socketError) {
    qDebug() << "Socket error:" << socket->errorString();
}
#endif
