#pragma once
#include <unordered_map>
#include <optional>
#include <variant>
#include <mutex>

#include "models/user.hpp"
#include "models/uuid.hpp"

class UserTable
{
public:
    UserTable() = default;

    // Getters
    [[nodiscard]] std::optional<const User::SharedPtr> get_by_uid(UUID user_uid);

    [[nodiscard]] std::optional<User::SharedPtr> get_mut_by_uid(UUID user_uid);

    [[nodiscard]] std::vector<UUID> get_uuids_matching_regex(std::string regex);

    // Setters
    std::variant<std::monostate, std::string> add_user(User::SharedPtr user);
    std::variant<std::monostate, std::string> remove_user(UUID user_uid);

private:
    std::unordered_map<UUID, User::SharedPtr> data;
    std::mutex mutex;
};