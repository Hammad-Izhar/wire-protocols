#pragma once
#include <unordered_map>
#include <stdint.h>
#include "models/message.hpp"

class message_db
{
public:
    message_db();
    // Getters
    [[nodiscard]] const std::unordered_map<uint64_t, Message> &get_data() const;
    [[nodiscard]] const Message get_by_uid(uint64_t message_snowflake) const;

    // Setters
    void add_message(Message &message);
    void remove_message(uint64_t message_snowflake);

private:
    std::unordered_map<uint64_t, Message> data;
};