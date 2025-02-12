#include <unordered_map>

#include "server/db/message_table.hpp"

MessageTable::MessageTable() {}

Message const MessageTable::get_by_uid(uint64_t message_snowflake) const
{
    return this->data.at(message_snowflake);
}

void MessageTable::add_message(Message &message)
{
    this->data.insert({message.get_snowflake(), message});
}

void MessageTable::remove_message(uint64_t message_snowflake)
{
    this->data.erase(message_snowflake);
}