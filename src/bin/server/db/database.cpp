#include "server/db/database.hpp"

Database::Database()
{
    this->users = std::make_unique<UserTable>();
    this->messages = std::make_unique<MessageTable>();
    this->channels = std::make_unique<ChannelTable>();
    this->passwords = std::make_unique<PasswordTable>();
}

Database &Database::get_instance()
{
    static Database instance;
    return instance;
}

const std::optional<const User::SharedPtr> Database::get_user_by_uid(UUID user_uid) const
{
    return this->users->get_by_uid(user_uid);
}

const std::optional<const Message::SharedPtr> Database::get_message_by_uid(uint64_t message_snowflake) const
{
    return this->messages->get_by_uid(message_snowflake);
}

const std::optional<const Channel::SharedPtr> Database::get_channel_by_uid(UUID channel_uid) const
{
    return this->channels->get_by_uid(channel_uid);
}

std::optional<User::SharedPtr> Database::get_mut_user_by_uid(UUID user_uid)
{
    return this->users->get_mut_by_uid(user_uid);
}

std::optional<Message::SharedPtr> Database::get_mut_message_by_uid(uint64_t message_snowflake)
{
    return this->messages->get_mut_by_uid(message_snowflake);
}

std::optional<Channel::SharedPtr> Database::get_mut_channel_by_uid(UUID channel_uid)
{
    return this->channels->get_mut_by_uid(channel_uid);
}

std::vector<UUID> Database::get_uuids_matching_regex(std::string regex) const
{
    return this->users->get_uuids_matching_regex(regex);
}

std::variant<std::monostate, std::string> Database::add_user(User::SharedPtr user)
{
    return this->users->add_user(user);
}

std::variant<std::monostate, std::string> Database::add_message(Message::SharedPtr message)
{
    std::optional<Channel::SharedPtr> channel = this->channels->get_mut_by_uid(message->get_channel_id());
    if (!channel.has_value())
    {
        return "Channel does not exist";
    }

    auto res = this->messages->add_message(message);
    if (std::holds_alternative<std::string>(res))
    {
        return std::get<std::string>(res);
    }

    channel.value()->add_message(message->get_snowflake());
    return {};
}

std::variant<std::monostate, std::string> Database::add_channel(Channel::SharedPtr channel)
{
    auto res = this->channels->add_channel(channel);
    if (std::holds_alternative<std::string>(res))
    {
        return std::get<std::string>(res);
    }
    // Add channel to all users
    for (auto &user_uid : channel->get_user_uids())
    {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value())
        {
            continue;
        }
        user.value()->add_channel(channel->get_uid());
    }

    return {};
}

std::variant<std::monostate, std::string> Database::add_user_to_channel(UUID user_uid, UUID channel_uid)
{
    std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
    if (!user.has_value())
    {
        return "User does not exist";
    }

    std::optional<Channel::SharedPtr> channel = this->channels->get_mut_by_uid(channel_uid);
    if (!channel.has_value())
    {
        return "Channel does not exist";
    }

    user.value()->add_channel(channel_uid);
    channel.value()->add_user(user_uid);
    return {};
}

std::variant<std::monostate, std::string> Database::remove_user(UUID user_uid)
{
    std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
    if (!user.has_value())
    {
        return "User does not exist";
    }

    for (auto &channel_uid : user.value()->get_channels())
    {
        std::optional<Channel::SharedPtr> channel_opt = this->channels->get_mut_by_uid(channel_uid);
        if (!channel_opt.has_value())
        {
            continue;
        }
        Channel::SharedPtr channel = channel_opt.value();
        channel->remove_user(user_uid);

        // For each message in the channel, remove those messages from the user
        for (auto &message_snowflake : channel->get_message_snowflakes())
        {
            std::optional<const Message::SharedPtr> message_opt = this->messages->get_by_uid(message_snowflake);
            if (!message_opt.has_value())
            {
                continue;
            }

            if (message_opt.value()->get_sender_id() == user_uid)
            {
                // Remove the message from the channel
                channel->remove_message(message_snowflake);
            }
        }
    }

    return this->users->remove_user(user_uid);
}

std::variant<std::monostate, std::string> Database::remove_message(uint64_t message_snowflake)
{
    std::optional<const Message::SharedPtr> message = this->messages->get_by_uid(message_snowflake);
    if (!message.has_value())
    {
        return "Message does not exist";
    }

    std::optional<Channel::SharedPtr> channel = this->channels->get_mut_by_uid(message.value()->get_channel_id());
    if (!channel.has_value())
    {
        return "Channel does not exist";
    }

    auto res = this->messages->remove_message(message_snowflake);
    if (std::holds_alternative<std::string>(res))
    {
        return std::get<std::string>(res);
    }

    channel.value()->remove_message(message_snowflake);
    return {};
}

std::variant<std::monostate, std::string> Database::remove_channel(UUID channel_uid)
{
    std::optional<const Channel::SharedPtr> channel = this->channels->get_by_uid(channel_uid);

    for (auto &user_uid : channel.value()->get_user_uids())
    {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value())
        {
            continue;
        }
        user.value()->remove_channel(channel_uid);
    }

    for (auto &message_snowflake : channel.value()->get_message_snowflakes())
    {
        std::optional<Message::SharedPtr> message = this->messages->get_mut_by_uid(message_snowflake);
        if (!message.has_value())
        {
            continue;
        }
        this->messages->remove_message(message_snowflake);
    }

    return this->channels->remove_channel(channel_uid);
}
