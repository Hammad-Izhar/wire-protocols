#pragma once
#include <stdint.h>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>

#include "models/channel.hpp"
#include "models/message.hpp"
#include "models/user.hpp"
#include "models/uuid.hpp"
#include "server/db/channel_table.hpp"
#include "server/db/message_table.hpp"
#include "server/db/password_table.hpp"
#include "server/db/user_table.hpp"

class Database {
   public:
    Database();

    static Database& get_instance();

    // Getters
    [[nodiscard]] const std::optional<const User::SharedPtr> get_user_by_uid(UUID user_uid) const;

    [[nodiscard]] const std::optional<const Message::SharedPtr> get_message_by_uid(
        uint64_t message_snowflake) const;

    [[nodiscard]] const std::optional<const Channel::SharedPtr> get_channel_by_uid(
        UUID channel_uid) const;

    [[nodiscard]] std::optional<User::SharedPtr> get_mut_user_by_uid(UUID user_uid);

    [[nodiscard]] std::optional<Message::SharedPtr> get_mut_message_by_uid(
        uint64_t message_snowflake);

    [[nodiscard]] std::optional<Channel::SharedPtr> get_mut_channel_by_uid(UUID channel_uid);

    [[nodiscard]] std::vector<UUID> get_uuids_matching_regex(std::string regex) const;

    [[nodiscard]] std::optional<UUID> get_uid_from_username(std::string username);

    std::variant<bool, std::string> verify_password(UUID& user_uid, std::string password);

    // Setters -- Add
    std::variant<std::monostate, std::string> add_user(User::SharedPtr user, std::string password);
    std::variant<std::monostate, std::string> add_message(Message::SharedPtr message);
    std::variant<std::monostate, std::string> add_channel(Channel::SharedPtr channel);
    std::variant<std::monostate, std::string> add_user_to_channel(UUID user_uid, UUID channel_uid);

    // Setters -- Remove
    std::variant<std::monostate, std::string> remove_user(UUID user_uid);
    std::variant<std::monostate, std::string> remove_message(uint64_t message_snowflake);
    std::variant<std::monostate, std::string> remove_channel(UUID channel_uid);

   private:
    std::unique_ptr<UserTable> users;
    std::unique_ptr<MessageTable> messages;
    std::unique_ptr<ChannelTable> channels;
    std::unique_ptr<PasswordTable> passwords;
};
