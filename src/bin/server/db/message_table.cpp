#include <unordered_map>

#include "server/db/message_table.hpp"

MessageTable::MessageTable() {}

std::optional<const Message::SharedPtr> MessageTable::get_by_uid(uint64_t message_snowflake) const
{
    return this->data.find(message_snowflake) != this->data.end() ? std::optional<const Message::SharedPtr>(this->data.at(message_snowflake)) : std::nullopt;
}

std::optional<Message::SharedPtr> MessageTable::get_mut_by_uid(uint64_t message_snowflake)
{
    return this->data.find(message_snowflake) != this->data.end() ? std::optional<Message::SharedPtr>(this->data.at(message_snowflake)) : std::nullopt;
}

std::variant<void, std::string> MessageTable::add_message(Message::SharedPtr message)
{
    this->data.insert({message->get_snowflake(), message});
}

std::variant<void, std::string> MessageTable::remove_message(uint64_t message_snowflake)
{
    this->data.erase(message_snowflake);
}