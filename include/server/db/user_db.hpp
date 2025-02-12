#pragma once
#include <unordered_map>

#include "models/user.hpp"
#include "models/uuid.hpp"

class user_db
{
public:
    user_db() = default;

    // Getters
    [[nodiscard]] const std::unordered_map<UUID, User> &get_data() const;
    [[nodiscard]] User &get_by_uid(UUID user_uid);

    // Setters
    void add_user(User &user);
    void remove_user(UUID user_uid);

private:
    std::unordered_map<UUID, User> data;
};