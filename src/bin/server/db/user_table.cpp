#include "server/db/user_table.hpp"

User &UserTable::get_by_uid(UUID user_uid)
{
    return this->data.at(user_uid);
}

void UserTable::add_user(User &user)
{
    this->data.insert({user.get_uid(), user});
}

void UserTable::remove_user(UUID user_uid)
{
    this->data.erase(user_uid);
}