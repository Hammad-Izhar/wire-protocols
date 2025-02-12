#include "models/channel.hpp"

Channel::Channel(std::string name, std::vector<UUID> user_uids) : name(std::move(name)), user_uids(std::move(user_uids))
{
    this->uid = UUID();
}

const UUID &Channel::get_uid() const
{
    return this->uid;
}

const std::string &Channel::get_name() const
{
    return this->name;
}

const std::vector<UUID> &Channel::get_user_uids() const
{
    return this->user_uids;
}

const std::vector<uint64_t> &Channel::get_message_snowflakes() const
{
    return this->message_snowflakes;
}

void Channel::set_name(std::string name)
{
    this->name = name;
}

void Channel::add_user(UUID user_uid)
{
    this->user_uids.push_back(user_uid);
}

void Channel::add_message(const uint64_t &message_snowflake)
{
    this->message_snowflakes.push_back(message_snowflake);
}

void Channel::remove_user(const UUID &user_uid)
{
    std::remove_if(this->user_uids.begin(), this->user_uids.end(), [&user_uid](const UUID &uid)
                   { return uid == user_uid; });
}

void Channel::remove_message(const uint64_t &message_snowflake)
{
    std::remove_if(this->message_snowflakes.begin(), this->message_snowflakes.end(), [&message_snowflake](const uint64_t &snowflake)
                   { return snowflake == message_snowflake; });
}