#include <string>
#include <regex>

#include "server/model/message_handlers.hpp"
#include "message/register_account_response.hpp"
#include "server/db/database.hpp"

void on_register_account(QTcpSocket *socket, RegisterAccountMessage &msg)
{
    Database &db = Database::get_instance();

    std::optional<UUID> user_id get_uid_from_username(msg.get_username());

    RegisterAccountResponse response;

    // If username exists, return
    if (user_id.has_value())
    {
        // Make error response
        response = RegisterAccountResponse("Username already exists");
    }
    else
    {
        // Try to add the password to the password table
        response = RegisterAccountResponse(db.add_user(msg.get_user(), msg.get_password()));
    }

    std::vector<uint8_t> buf;
    response.serialize_msg(buf);
    socket->write(reinterpret_cast<const char *>(buf.data()), buf.size());
    socket->flush();
}

void delete_account(DeleteAccountMessage &msg)
{
    Database &db = Database::get_instance();

    std::optional<UUID> user_uid = db.get_uid_from_username(msg.get_username());
    if (!user_uid.has_value())
    {
        return;
    }
    // verify the password
    std::variant<bool, std::string> res = db.verify_password(user_uid.value(), msg.get_password());
    if (std::holds_alternative<std::string>(res))
    {
        return;
    }
    if (!std::get<bool>(res))
    {
        return;
    }

    db.remove_user(user_uid.value());
}

void list_accounts(ListAccountsMessage &msg)
{
    std::string regex_string = msg.get_regex();
    std::regex re(regex_string);
    Database &db = Database::get_instance();

    std::vector<UUID> uuids = db.get_uuids_matching_regex(regex_string);

    std::vector<User::SharedPtr> users = {};
    for (const auto &uuid : uuids)
    {
        std::optional<const User::SharedPtr> user = db.get_user_by_uid(uuid);
        if (user.has_value())
        {
            users.push_back(user.value());
        }
    }
}

void send_message(SendMessageMessage &msg)
{
    Database &db = Database::get_instance();

    std::shared_ptr<Message> message = std::make_shared<Message>(msg.get_channel_uid(), msg.get_sender_uid(), msg.get_text());

    db.add_message(message);
}

void delete_message(DeleteMessageMessage &msg)
{
    Database &db = Database::get_instance();
    db.remove_message(msg.get_message_snowflake());
}
