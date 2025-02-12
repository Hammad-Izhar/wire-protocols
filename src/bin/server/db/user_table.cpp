#include "server/db/user_table.hpp"

std::optional<const User::SharedPtr> UserTable::get_by_uid(UUID user_uid)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(user_uid) != this->data.end() ? std::optional<const User::SharedPtr>(this->data.at(user_uid)) : std::nullopt;
}

std::optional<User::SharedPtr> UserTable::get_mut_by_uid(UUID user_uid)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(user_uid) != this->data.end() ? std::optional<User::SharedPtr>(this->data.at(user_uid)) : std::nullopt;
}

std::variant<void, std::string> UserTable::add_user(User::SharedPtr user)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.insert({user->get_uid(), user});
}

std::variant<void, std::string> UserTable::remove_user(UUID user_uid)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.erase(user_uid);
}