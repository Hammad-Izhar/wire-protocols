#include "server/db/database.hpp"

Database::Database()
{
    this->users = UserTable();
    this->messages = MessageTable();
    this->channels = ChannelTable();
}

User &Database::get_user_by_uid(UUID user_uid)
{
    return this->users.get_by_uid(user_uid);
}

const Message &Database::get_message_by_uid(uint64_t message_snowflake) const
{
    return this->messages.get_by_uid(message_snowflake);
}

const Channel &Database::get_channel_by_uid(UUID channel_uid) const
{
    return this->channels.get_by_uid(channel_uid);
}

void Database::add_user(User &user)
{
    this->users.add_user(user);
}

void Database::add_message(Message &message)
{
    this->messages.add_message(message);
    // Add message snowflake to channel
    this->channels.get_by_uid(message.get_channel_id()).add_message(message.get_snowflake());
}

void Database::add_channel(Channel &channel)
{
    this->channels.add_channel(channel);
    // Add channel to all users
    for (auto &user_uid : channel.get_user_uids())
    {
        this->users.get_by_uid(user_uid).add_channel(channel.get_uid());
    }
}

void Database::remove_user(UUID user_uid)
{
    this->users.remove_user(user_uid);
    // Remove user from all channels
    for (auto &channel_uid : this->users.get_by_uid(user_uid).get_channels())
    {
        this->channels.get_by_uid(channel_uid).remove_user(user_uid);
        // For each message in the channel, remove those messages from the user
        for (auto &message_snowflake : this->channels.get_by_uid(channel_uid).get_message_snowflakes())
        {
            UUID sender_id = this->messages.get_by_uid(message_snowflake).get_sender_id();
            if (sender_id == user_uid)
            {
                // Remove the message from the channel
                this->channels.get_by_uid(channel_uid).remove_message(message_snowflake);
            }
        }
    }
}

void Database::remove_message(uint64_t message_snowflake)
{
    this->messages.remove_message(message_snowflake);
    Channel channel = this->channels.get_by_uid(this->messages.get_by_uid(message_snowflake).get_channel_id());
    channel.remove_message(message_snowflake);
}

void Database::remove_channel(UUID channel_uid)
{
    this->channels.remove_channel(channel_uid);
    // Remove all messages in the channel
    for (auto &message_snowflake : this->channels.get_by_uid(channel_uid).get_message_snowflakes())
    {
        this->messages.remove_message(message_snowflake);
    }

    // Remove channel from all users
    for (auto &user_uid : this->channels.get_by_uid(channel_uid).get_user_uids())
    {
        this->users.get_by_uid(user_uid).remove_channel(channel_uid);
    }
}
