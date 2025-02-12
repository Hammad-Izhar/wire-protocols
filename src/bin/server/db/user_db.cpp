#include "server/db/user_db.hpp"

const std::unordered_map<UUID, User> &user_db::get_data() const
{
    return this->data;
}

User &user_db::get_by_uid(UUID user_uid)
{
    return this->data.at(user_uid);
}

void user_db::add_user(User &user)
{
    this->data.insert({user.get_uid(), user});
}

void user_db::remove_user(UUID user_uid)
{
    this->data.erase(user_uid);
}