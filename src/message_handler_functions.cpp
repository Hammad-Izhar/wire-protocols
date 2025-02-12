#include "message_handler_functions.hpp"
#include "db/database.hpp"
#include <string>
#include <regex>

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