#include <unordered_map>

#include "server/db/message_table.hpp"

std::optional<const Message::SharedPtr> MessageTable::get_by_uid(uint64_t message_snowflake)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(message_snowflake) != this->data.end() ? std::optional<const Message::SharedPtr>(this->data.at(message_snowflake)) : std::nullopt;
}

std::optional<Message::SharedPtr> MessageTable::get_mut_by_uid(uint64_t message_snowflake)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(message_snowflake) != this->data.end() ? std::optional<Message::SharedPtr>(this->data.at(message_snowflake)) : std::nullopt;
}

std::variant<std::monostate, std::string> MessageTable::add_message(Message::SharedPtr message)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.insert({message->get_snowflake(), message});

    return {};
}

std::variant<std::monostate, std::string> MessageTable::remove_message(uint64_t message_snowflake)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.erase(message_snowflake);

    return {};
}