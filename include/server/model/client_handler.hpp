#pragma once
#include <QTcpSocket>
#include <optional>

#include "models/user.hpp"

class ClientHandler : public QObject {
    Q_OBJECT

   public:
    explicit ClientHandler(qintptr socket_descriptor, QObject* parent = nullptr);

    void set_authenticated_user(const User::SharedPtr user);

   private:
    QTcpSocket* socket;
    qintptr socket_descriptor;
    std::optional<User::SharedPtr> authenticated_user;

   public slots:
    void handle_client();

   private slots:
    void on_write_data(std::vector<uint8_t> data);

    void on_read_data();

    void on_disconnected();

    void on_message_received(std::variant<Message::SharedPtr, std::string> message);

    // void on_message_deleted(std::variant<Message::SharedPtr, std::string> message);

    void on_channel_added(std::variant<Channel::SharedPtr, std::string> channel);

    // void on_channel_removed(std::variant<Channel::SharedPtr, std::string> channel);

   signals:
    void finished();
};
