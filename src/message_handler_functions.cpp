#include "message_handler_functions.hpp"
#include "db/database.hpp"
#include <string>
#include <regex>

void register_account(RegisterAccountMessage &msg)
{
    Database &db = Database::get_instance();
    User::SharedPtr user = std::make_shared<User>(msg.get_username(), msg.get_password(), msg.get_display_name());
    db.add_user(user);
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