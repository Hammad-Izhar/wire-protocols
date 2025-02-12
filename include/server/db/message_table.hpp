#pragma once
#include <unordered_map>
#include <stdint.h>

#include "models/message.hpp"

class MessageTable
{
public:
    MessageTable();
    // Getters
    [[nodiscard]] const Message get_by_uid(uint64_t message_snowflake) const;

    // Setters
    void add_message(Message &message);
    void remove_message(uint64_t message_snowflake);

private:
    std::unordered_map<uint64_t, Message> data;
};