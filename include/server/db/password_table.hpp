#pragma once
#include <stdint.h>
#include <array>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

#include "models/uuid.hpp"

class PasswordTable {
   public:
    PasswordTable() = default;
    std::variant<bool, std::string> verify_password(UUID& user_uid, std::string password);

    // Setters
    std::variant<std::monostate, std::string> add_password(UUID& user_uid, std::string password);
    std::variant<std::monostate, std::string> remove_password(UUID& user_uid);

   private:
    std::unordered_map<UUID, std::pair<std::string, std::string>> data;
    std::mutex mutex;

    std::string sha256(const std::string& str);
    std::string generate_salt();
};
