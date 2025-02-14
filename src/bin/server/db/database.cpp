#include <QDebug>

#include <qtmetamacros.h>
#include "models/message.hpp"
#include "server/db/database.hpp"

Database::Database() {
    this->users = std::make_unique<UserTable>();
    this->messages = std::make_unique<MessageTable>();
    this->channels = std::make_unique<ChannelTable>();
    this->passwords = std::make_unique<PasswordTable>();
}

Database& Database::get_instance() {
    static Database instance;
    return instance;
}

const std::optional<const User::SharedPtr> Database::get_user_by_uid(UUID user_uid) const {
    return this->users->get_by_uid(user_uid);
}

const std::optional<const Message::SharedPtr> Database::get_message_by_uid(
    uint64_t message_snowflake) const {
    return this->messages->get_by_uid(message_snowflake);
}

const std::optional<const Channel::SharedPtr> Database::get_channel_by_uid(UUID channel_uid) const {
    return this->channels->get_by_uid(channel_uid);
}

std::optional<User::SharedPtr> Database::get_mut_user_by_uid(UUID user_uid) {
    return this->users->get_mut_by_uid(user_uid);
}

std::optional<Message::SharedPtr> Database::get_mut_message_by_uid(uint64_t message_snowflake) {
    return this->messages->get_mut_by_uid(message_snowflake);
}

std::optional<Channel::SharedPtr> Database::get_mut_channel_by_uid(UUID channel_uid) {
    return this->channels->get_mut_by_uid(channel_uid);
}

std::vector<UUID> Database::get_uuids_matching_regex(std::string regex) const {
    return this->users->get_uuids_matching_regex(regex);
}

std::optional<UUID> Database::get_uid_from_username(std::string username) {
    return this->users->get_uid_from_username(username);
}

std::variant<bool, std::string> Database::verify_password(UUID& user_uid, std::string password) {
    return this->passwords->verify_password(user_uid, password);
}

std::variant<std::monostate, std::string> Database::add_user(User::SharedPtr user,
                                                             std::string password) {
    // Add the password
    UUID user_uid = user->get_uid();
    std::variant<std::monostate, std::string> res =
        this->passwords->add_password(user_uid, password);
    if (std::holds_alternative<std::string>(res)) {
        return std::get<std::string>(res);
    }
    // Add the user
    return this->users->add_user(user);
}

std::variant<Message::SharedPtr, std::string> Database::add_message(UUID sender_uid,
                                                                    UUID channel_uid,
                                                                    std::string content) {
    std::optional<Channel::SharedPtr> channel = this->channels->get_mut_by_uid(channel_uid);
    if (!channel.has_value()) {
        return "Channel does not exist";
    }

    auto res = this->messages->add_message(sender_uid, channel_uid, content);
    if (std::holds_alternative<std::string>(res)) {
        return std::get<std::string>(res);
    }
    Message::SharedPtr message = std::get<Message::SharedPtr>(res);

    channel.value()->add_message(message->get_snowflake());
    for (auto user_uid : channel.value()->get_user_uids()) {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value()) {
            continue;
        }
        emit user.value()->message_received(message);
    }

    return message;
}

std::variant<Channel::SharedPtr, std::string> Database::add_channel(std::string channel_name,
                                                                    std::vector<UUID> members) {
    auto res = this->channels->add_channel(channel_name, members);
    if (std::holds_alternative<std::string>(res)) {
        return std::get<std::string>(res);
    }
    Channel::SharedPtr channel = std::get<Channel::SharedPtr>(res);

    for (auto& user_uid : channel->get_user_uids()) {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value()) {
            continue;
        }
        user.value()->add_channel(channel->get_uid());
        emit user.value()->channel_added(channel);
    }

    return channel;
}

std::variant<std::monostate, std::string> Database::add_user_to_channel(UUID user_uid,
                                                                        UUID channel_uid) {
    std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
    if (!user.has_value()) {
        return "User does not exist";
    }

    std::optional<Channel::SharedPtr> channel = this->channels->get_mut_by_uid(channel_uid);
    if (!channel.has_value()) {
        return "Channel does not exist";
    }

    user.value()->add_channel(channel_uid);
    channel.value()->add_user(user_uid);
    return {};
}

std::variant<User::SharedPtr, std::string> Database::remove_user(UUID user_uid) {
    std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
    if (!user.has_value()) {
        return "User does not exist";
    }

    for (auto& channel_uid : user.value()->get_channels()) {
        std::optional<Channel::SharedPtr> channel_opt = this->channels->get_mut_by_uid(channel_uid);
        if (!channel_opt.has_value()) {
            continue;
        }
        Channel::SharedPtr channel = channel_opt.value();
        channel->remove_user(user_uid);

        // For each message in the channel, remove those messages from the user
        for (auto& message_snowflake : channel->get_message_snowflakes()) {
            std::optional<const Message::SharedPtr> message_opt =
                this->messages->get_by_uid(message_snowflake);
            if (!message_opt.has_value()) {
                continue;
            }

            if (message_opt.value()->get_sender_id() == user_uid) {
                channel->remove_message(message_snowflake);
                for (auto& user_uid : channel->get_user_uids()) {
                    std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
                    if (!user.has_value()) {
                        continue;
                    }
                    emit user.value()->message_deleted(message_opt.value());
                }
            }
        }
    }

    return this->users->remove_user(user_uid);
}

std::variant<std::monostate, std::string> Database::remove_message(uint64_t message_snowflake) {
    std::optional<const Message::SharedPtr> message = this->messages->get_by_uid(message_snowflake);
    if (!message.has_value()) {
        return "Message does not exist";
    }

    std::optional<Channel::SharedPtr> channel =
        this->channels->get_mut_by_uid(message.value()->get_channel_id());
    if (!channel.has_value()) {
        return "Channel does not exist";
    }

    auto res = this->messages->remove_message(message_snowflake);
    if (std::holds_alternative<std::string>(res)) {
        return std::get<std::string>(res);
    }

    for (auto user_uid : channel.value()->get_user_uids()) {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value()) {
            continue;
        }
        emit user.value()->message_deleted(message.value());
    }

    channel.value()->remove_message(message_snowflake);
    return {};
}

std::variant<std::monostate, std::string> Database::remove_channel(UUID channel_uid) {
    std::optional<const Channel::SharedPtr> channel = this->channels->get_by_uid(channel_uid);

    for (auto& user_uid : channel.value()->get_user_uids()) {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value()) {
            continue;
        }
        user.value()->remove_channel(channel_uid);
    }

    for (auto& message_snowflake : channel.value()->get_message_snowflakes()) {
        std::optional<Message::SharedPtr> message =
            this->messages->get_mut_by_uid(message_snowflake);
        if (!message.has_value()) {
            continue;
        }
        this->messages->remove_message(message_snowflake);
    }

    return this->channels->remove_channel(channel_uid);
}
