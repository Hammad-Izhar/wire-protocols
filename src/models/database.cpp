// #include "models/database.hpp"

// database::database() {}

// const user_db &database::get_users() const
// {
//     return this->users;
// }

// const message_db &database::get_messages() const
// {
//     return this->messages;
// }

// const channel_db &database::get_channels() const
// {
//     return this->channels;
// }

// const User &database::get_user_by_uid(std::array<uint8_t,16> user_uid) const
// {
//     return this->users.get_by_uid(user_uid);
// }

// const Message &database::get_message_by_uid(uint64_t message_snowflake) const
// {
//     return this->messages.get_by_uid(message_snowflake);
// }

// const Channel &database::get_channel_by_uid(std::array<uint8_t,16> channel_uid) const
// {
//     return this->channels.get_by_uid(channel_uid);
// }

// void database::add_user(User &user)
// {
//     this->users.add_user(user);
// }

// void database::add_message(Message &message)
// {
//     this->messages.add_message(message);
// }

// void database::add_channel(Channel &channel)
// {
//     this->channels.add_channel(channel);
// }

// void database::remove_user(std::array<uint8_t,16> user_uid)
// {
//     this->users.remove_user(user_uid);
// }

// void database::remove_message(uint64_t message_snowflake)
// {
//     this->messages.remove_message(message_snowflake);
// }

// void database::remove_channel(std::array<uint8_t,16> channel_uid)
// {
//     this->channels.remove_channel(channel_uid);
//     // Remove all messages in the channel
//     for (auto &message_snowflake : this->channels.get_by_uid(channel_uid).get_message_snowflakes()) {
//         this->messages.remove_message(message_snowflake);
//     }
// }


