// #pragma once
// #include <unordered_map>
// #include "models/message_db.hpp"

// message_db::message_db() {}

// const std::unordered_map<uint64_t, Message> &message_db::get_data() const
// {
//     return this->data;
// }

// Message message_db::get_by_uid(uint64_t message_snowflake) const
// {
//     return this->data.at(message_snowflake);
// }

// void message_db::add_message(Message &message)
// {
//     this->data.insert({message.get_snowflake(), message});
// }

// void message_db::remove_message(uint64_t message_snowflake)
// {
//     this->data.erase(message_snowflake);
// }