#pragma once
#include <unordered_map>
#include <stdint.h>
#include <optional>
#include <variant>
#include <mutex>

#include "models/message.hpp"

class MessageTable
{
public:
    MessageTable() = default;
    // Getters
    [[nodiscard]] std::optional<const Message::SharedPtr> get_by_uid(uint64_t message_snowflake);

    [[nodiscard]] std::optional<Message::SharedPtr> get_mut_by_uid(uint64_t message_snowflake);

    // Setters
    std::variant<std::monostate, std::string> add_message(Message::SharedPtr message);
    std::variant<std::monostate, std::string> remove_message(uint64_t message_snowflake);

private:
    std::unordered_map<uint64_t, Message::SharedPtr> data;
    std::mutex mutex;
};