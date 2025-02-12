#pragma once
#include <unordered_map>

#include "models/user.hpp"
#include "models/uuid.hpp"

class UserTable
{
public:
    UserTable() = default;

    // Getters
    [[nodiscard]] User &get_by_uid(UUID user_uid);

    // Setters
    void add_user(User &user);
    void remove_user(UUID user_uid);

private:
    std::unordered_map<UUID, User> data;
};