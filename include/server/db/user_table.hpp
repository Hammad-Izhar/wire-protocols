#pragma once
#include <mutex>
#include <optional>
#include <unordered_map>
#include <variant>

#include "models/user.hpp"
#include "models/uuid.hpp"

class UserTable {
   public:
    UserTable() = default;

    // Getters
    [[nodiscard]] std::optional<const User::SharedPtr> get_by_uid(UUID user_uid);

    [[nodiscard]] std::optional<User::SharedPtr> get_mut_by_uid(UUID user_uid);

    [[nodiscard]] std::variant<std::vector<UUID>, std::string> get_uuids_matching_regex(std::string regex);

    [[nodiscard]] std::optional<UUID> get_uid_from_username(std::string username);

    // Setters
    std::variant<std::monostate, std::string> add_user(User::SharedPtr user);
    std::variant<User::SharedPtr, std::string> remove_user(UUID user_uid);

   private:
    std::unordered_map<UUID, User::SharedPtr> data;
    std::mutex mutex;
};