#pragma once
#include <stdint.h>
#include <memory>
#include <optional>
#include <variant>

#include "models/channel.hpp"
#include "models/message.hpp"
#include "models/user.hpp"
#include "models/uuid.hpp"
#include "server/db/channel_table.hpp"
#include "server/db/message_table.hpp"
#include "server/db/password_table.hpp"
#include "server/db/user_table.hpp"

/**
 * @brief Provides a unified interface for interacting with the application's database.
 *
 * The Database class encapsulates various tables (users, messages, channels, and passwords)
 * and provides methods for retrieving, adding, and removing records. It follows the singleton
 * pattern to ensure that only one instance of the Database exists.
 */
class Database {
   public:
    /**
     * @brief Constructs a new Database instance.
     *
     * Initializes the underlying tables for users, messages, channels, and passwords.
     */
    Database();

    /**
     * @brief Retrieves the singleton instance of the Database.
     *
     * @return A reference to the singleton Database instance.
     */
    static Database& get_instance();

    // Getters

    /**
     * @brief Retrieves a read-only user by its unique identifier.
     *
     * @param user_uid The UUID of the user to retrieve.
     * @return An optional containing a constant shared pointer to the user if found, or std::nullopt otherwise.
     */
    [[nodiscard]] const std::optional<const User::SharedPtr> get_user_by_uid(UUID user_uid) const;

    /**
     * @brief Retrieves a read-only message by its unique snowflake identifier.
     *
     * @param message_snowflake The unique snowflake identifier of the message.
     * @return An optional containing a constant shared pointer to the message if found, or std::nullopt otherwise.
     */
    [[nodiscard]] const std::optional<const Message::SharedPtr> get_message_by_uid(
        uint64_t message_snowflake) const;

    /**
     * @brief Retrieves a read-only channel by its unique identifier.
     *
     * @param channel_uid The UUID of the channel to retrieve.
     * @return An optional containing a constant shared pointer to the channel if found, or std::nullopt otherwise.
     */
    [[nodiscard]] const std::optional<const Channel::SharedPtr> get_channel_by_uid(
        UUID channel_uid) const;

    /**
     * @brief Retrieves a mutable user by its unique identifier.
     *
     * @param user_uid The UUID of the user to retrieve.
     * @return An optional containing a mutable shared pointer to the user if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<User::SharedPtr> get_mut_user_by_uid(UUID user_uid);

    /**
     * @brief Retrieves a mutable message by its unique snowflake identifier.
     *
     * @param message_snowflake The unique snowflake identifier of the message.
     * @return An optional containing a mutable shared pointer to the message if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<Message::SharedPtr> get_mut_message_by_uid(
        uint64_t message_snowflake);

    /**
     * @brief Retrieves a mutable channel by its unique identifier.
     *
     * @param channel_uid The UUID of the channel to retrieve.
     * @return An optional containing a mutable shared pointer to the channel if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<Channel::SharedPtr> get_mut_channel_by_uid(UUID channel_uid);

    /**
     * @brief Retrieves UUIDs that match a given regular expression.
     *
     * Searches through user records to find UUIDs whose associated data match the provided regex.
     *
     * @param regex The regular expression to match.
     * @return A variant containing a vector of matching UUIDs on success, or an error message string on failure.
     */
    [[nodiscard]] std::variant<std::vector<UUID>, std::string> get_uuids_matching_regex(std::string regex) const;

    /**
     * @brief Retrieves a user's UUID by their username.
     *
     * @param username The username to search for.
     * @return An optional containing the UUID if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<UUID> get_uid_from_username(std::string username);

    /**
     * @brief Verifies a user's password.
     *
     * Checks if the provided password is valid for the user with the given UUID.
     *
     * @param user_uid A reference to the user's UUID.
     * @param password The password to verify.
     * @return A variant containing true on success or an error message string on failure.
     */
    std::variant<bool, std::string> verify_password(UUID& user_uid, std::string password);

    // Setters -- Add

    /**
     * @brief Adds a new user to the database.
     *
     * Stores the user along with their password.
     *
     * @param user A shared pointer to the User to add.
     * @param password The password associated with the user.
     * @return A variant containing std::monostate on success or an error message string on failure.
     */
    std::variant<std::monostate, std::string> add_user(User::SharedPtr user, std::string password);

    /**
     * @brief Adds a new message to the database.
     *
     * Stores a new message from the specified sender in the specified channel with the given content.
     *
     * @param sender_uid The UUID of the sender.
     * @param channel_uid The UUID of the channel.
     * @param content The content of the message.
     * @return A variant containing a shared pointer to the newly created Message on success, or an error message string on failure.
     */
    std::variant<Message::SharedPtr, std::string> add_message(UUID sender_uid,
                                                              UUID channel_uid,
                                                              std::string content);

    /**
     * @brief Adds a new channel to the database.
     *
     * Creates and stores a new channel with the specified name and initial members.
     *
     * @param channel_name The name of the channel.
     * @param members A vector of UUIDs representing the initial members of the channel.
     * @return A variant containing a shared pointer to the newly created Channel on success, or an error message string on failure.
     */
    std::variant<Channel::SharedPtr, std::string> add_channel(std::string channel_name,
                                                              std::vector<UUID> members);

    /**
     * @brief Adds a user to an existing channel.
     *
     * Associates the user with the specified channel.
     *
     * @param user_uid The UUID of the user to add.
     * @param channel_uid The UUID of the channel.
     * @return A variant containing std::monostate on success or an error message string on failure.
     */
    std::variant<std::monostate, std::string> add_user_to_channel(UUID user_uid, UUID channel_uid);

    // Setters -- Remove

    /**
     * @brief Removes a user from the database.
     *
     * Deletes the user with the specified UUID.
     *
     * @param user_uid The UUID of the user to remove.
     * @return A variant containing a shared pointer to the removed User on success, or an error message string on failure.
     */
    std::variant<User::SharedPtr, std::string> remove_user(UUID user_uid);

    /**
     * @brief Removes a message from the database.
     *
     * Deletes the message with the specified snowflake identifier.
     *
     * @param message_snowflake The unique snowflake identifier of the message to remove.
     * @return A variant containing std::monostate on success or an error message string on failure.
     */
    std::variant<std::monostate, std::string> remove_message(uint64_t message_snowflake);

    /**
     * @brief Removes a channel from the database.
     *
     * Deletes the channel with the specified UUID.
     *
     * @param channel_uid The UUID of the channel to remove.
     * @return A variant containing std::monostate on success or an error message string on failure.
     */
    std::variant<std::monostate, std::string> remove_channel(UUID channel_uid);

   private:
    /// Pointer to the user table.
    std::unique_ptr<UserTable> users;
    /// Pointer to the message table.
    std::unique_ptr<MessageTable> messages;
    /// Pointer to the channel table.
    std::unique_ptr<ChannelTable> channels;
    /// Pointer to the password table.
    std::unique_ptr<PasswordTable> passwords;
};
