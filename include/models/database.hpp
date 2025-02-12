// #pragma once
// #include <unordered_map>
// #include <stdint.h>
// #include "models/user_db.hpp"
// #include "models/message_db.hpp"
// #include "models/channel_db.hpp"

// class database
// {
// public:
//     // Getters
//     [[nodiscard]] const user_db &get_users() const;
//     [[nodiscard]] const message_db &get_messages() const;
//     [[nodiscard]] const channel_db &get_channels() const;

//     [[nodiscard]] const User &get_user_by_uid(std::array<uint8_t,16> user_uid) const;
//     [[nodiscard]] const Message &get_message_by_uid(uint64_t message_snowflake) const;
//     [[nodiscard]] const Channel &get_channel_by_uid(std::array<uint8_t,16> channel_uid) const;

//     // Setters -- Add
//     void add_user(User &user);
//     void add_message(Message &message);
//     void add_channel(Channel &channel);

//     // Setters -- Remove
//     void remove_user(std::array<uint8_t,16> user_uid);
//     void remove_message(uint64_t message_snowflake);
//     void remove_channel(std::array<uint8_t,16> channel_uid);

// private:
//     user_db users;
//     message_db messages;
//     channel_db channels;
// };

