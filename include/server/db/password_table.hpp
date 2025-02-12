#pragma once
#include <unordered_map>
#include <stdint.h>
#include <optional>
#include <utility>
#include <array>
#include <string>
#include <variant>
#include <mutex>

#include "models/uuid.hpp"

class PasswordTable
{
public:
    PasswordTable() = default;
    std::variant<bool, std::string> verify_password(UUID &user_uid, std::string password);

    // Setters
    std::variant<std::monostate, std::string> add_password(UUID &user_uid, std::string password);
    std::variant<std::monostate, std::string> remove_password(UUID &user_uid);

private:
    std::unordered_map<UUID, std::pair<std::string, std::string>> data;
    std::mutex mutex;

    std::string sha256(const std::string &str);
    std::string generate_salt();
};
