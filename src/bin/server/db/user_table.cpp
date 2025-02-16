#include "server/db/user_table.hpp"
#include <regex>

std::optional<const User::SharedPtr> UserTable::get_by_uid(UUID user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(user_uid) != this->data.end()
               ? std::optional<const User::SharedPtr>(this->data.at(user_uid))
               : std::nullopt;
}

std::optional<User::SharedPtr> UserTable::get_mut_by_uid(UUID user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(user_uid) != this->data.end()
               ? std::optional<User::SharedPtr>(this->data.at(user_uid))
               : std::nullopt;
}

std::variant<std::vector<UUID>, std::string> UserTable::get_uuids_matching_regex(std::string regex) {
    std::lock_guard<std::mutex> lock(this->mutex);
    std::vector<UUID> uuids;
    std::regex re;

    try {
        re = std::regex(regex);
    } catch (const std::regex_error& e) {
        return std::string("Regex error: ") + e.what();
    }

    for (const auto& [uid, user] : this->data) {
        if (std::regex_match(user->get_username(), re)) {
            uuids.push_back(uid);
        }
    }
    return uuids;
}

std::optional<UUID> UserTable::get_uid_from_username(std::string username) {
    std::lock_guard<std::mutex> lock(this->mutex);
    for (const auto& [uid, user] : this->data) {
        if (user->get_username() == username) {
            return uid;
        }
    }
    return std::nullopt;
}

std::variant<std::monostate, std::string> UserTable::add_user(User::SharedPtr user) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.insert({user->get_uid(), user});

    return {};
}

std::variant<User::SharedPtr, std::string> UserTable::remove_user(UUID user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    if (this->data.find(user_uid) == this->data.end()) {
        return "User does not exist";
    }

    User::SharedPtr user = this->data[user_uid];
    this->data.erase(user_uid);

    return user;
}