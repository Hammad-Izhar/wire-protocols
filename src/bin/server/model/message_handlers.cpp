#include <QTcpSocket>
#include <string>

#include "message/delete_account.hpp"
#include "message/delete_message.hpp"
#include "message/list_accounts.hpp"
#include "message/list_accounts_response.hpp"
#include "message/login.hpp"
#include "message/login_response.hpp"
#include "message/register_account.hpp"
#include "message/register_account_response.hpp"
#include "message/send_message.hpp"
#include "models/message_handler.hpp"
#include "server/db/database.hpp"
#include "server/model/client_handler.hpp"
#include "server/model/message_handlers.hpp"

void on_register_account(QTcpSocket* socket, RegisterAccountMessage& msg) {
    Database& db = Database::get_instance();

    std::optional<UUID> user_id = db.get_uid_from_username(msg.get_username());

    RegisterAccountResponse response;

    // If username exists, return
    if (user_id.has_value()) {
        // Make error response
        response = RegisterAccountResponse("Username already exists");
    } else {
        // Try to add the password to the password table
        User::SharedPtr user = std::make_shared<User>(msg.get_username(), msg.get_display_name());
        response = RegisterAccountResponse(db.add_user(user, msg.get_password()));
    }

    std::vector<uint8_t> buf;
    response.serialize_msg(buf);
    socket->write(reinterpret_cast<const char*>(buf.data()), buf.size());
    socket->flush();
}

void on_login(QTcpSocket* socket, LoginMessage& msg) {
    Database& db = Database::get_instance();
    ClientHandler* client = qobject_cast<ClientHandler*>(socket->parent());
    if (client == nullptr) {
        qDebug() << "ClientHandler is null";
        return;
    }

    LoginResponse response;
    std::optional<UUID> user_uid = db.get_uid_from_username(msg.get_username());
    if (!user_uid.has_value()) {
        response = LoginResponse("Username does not exist");
    } else {
        std::variant<bool, std::string> res =
            db.verify_password(user_uid.value(), msg.get_password());
        if (std::holds_alternative<std::string>(res)) {
            response = LoginResponse(std::get<std::string>(res));
        } else if (!std::get<bool>(res)) {
            response = LoginResponse("Incorrect password");
        } else {
            User::SharedPtr user = db.get_user_by_uid(user_uid.value()).value();
            client->setAuthenticatedUser(user);
            response = LoginResponse(user);
        }
    }

    std::vector<uint8_t> buf;
    response.serialize_msg(buf);
    socket->write(reinterpret_cast<const char*>(buf.data()), buf.size());
    socket->flush();
}

void on_list_accounts(QTcpSocket* socket, ListAccountsMessage& msg) {
    std::string regex_string = msg.get_regex();
    Database& db = Database::get_instance();

    // TODO: Paginate this
    std::vector<UUID> uuids = db.get_uuids_matching_regex(regex_string);
    std::vector<User::SharedPtr> users = {};
    for (const auto& uuid : uuids) {
        std::optional<const User::SharedPtr> user = db.get_user_by_uid(uuid);
        if (user.has_value()) {
            users.push_back(user.value());
        }
    }

    ListAccountsResponse response(users);
    std::vector<uint8_t> buf;
    response.serialize_msg(buf);
    socket->write(reinterpret_cast<const char*>(buf.data()), buf.size());
    socket->flush();
}

void on_delete_account(QTcpSocket* socket, DeleteAccountMessage& msg) {
    Database& db = Database::get_instance();

    std::optional<UUID> user_uid = db.get_uid_from_username(msg.get_username());
    if (!user_uid.has_value()) {
        return;
    }
    // verify the password
    std::variant<bool, std::string> res = db.verify_password(user_uid.value(), msg.get_password());
    if (std::holds_alternative<std::string>(res)) {
        return;
    }
    if (!std::get<bool>(res)) {
        return;
    }

    db.remove_user(user_uid.value());
}

void on_send_message(QTcpSocket* socket, SendMessageMessage& msg) {
    Database& db = Database::get_instance();

    std::shared_ptr<Message> message =
        std::make_shared<Message>(msg.get_channel_uid(), msg.get_sender_uid(), msg.get_text());

    db.add_message(message);
}

void on_delete_message(QTcpSocket* socket, DeleteMessageMessage& msg) {
    Database& db = Database::get_instance();
    db.remove_message(msg.get_message_snowflake());
}

void init_message_handlers() {
    MessageHandler& messageHandler = MessageHandler::get_instance();

    messageHandler.register_handler<RegisterAccountMessage>(&on_register_account);
    messageHandler.register_handler<LoginMessage>(&on_login);
    messageHandler.register_handler<ListAccountsMessage>(&on_list_accounts);
    messageHandler.register_handler<DeleteAccountMessage>(&on_delete_account);
    messageHandler.register_handler<SendMessageMessage>(&on_send_message);
    messageHandler.register_handler<DeleteMessageMessage>(&on_delete_message);
}