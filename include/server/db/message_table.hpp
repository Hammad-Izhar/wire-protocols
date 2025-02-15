#pragma once
#include <stdint.h>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <variant>

#include "models/message.hpp"

/**
 * @brief Manages a collection of messages.
 *
 * The MessageTable class provides a thread-safe interface for storing, retrieving, and managing
 * messages identified by their unique snowflake identifiers. It supports both read-only and mutable
 * access, as well as methods to add and remove messages.
 */
class MessageTable {
   public:
    /**
     * @brief Default constructor.
     *
     * Constructs a new MessageTable instance with an empty collection of messages.
     */
    MessageTable() = default;

    /**
     * @brief Retrieves a message by its unique snowflake identifier (read-only).
     *
     * Searches the table for a message with the specified snowflake identifier.
     *
     * @param message_snowflake The unique snowflake identifier of the message.
     * @return An optional containing a constant shared pointer to the message if found; std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<const Message::SharedPtr> get_by_uid(uint64_t message_snowflake);

    /**
     * @brief Retrieves a message by its unique snowflake identifier (mutable).
     *
     * Searches the table for a message with the specified snowflake identifier and returns a mutable shared pointer.
     *
     * @param message_snowflake The unique snowflake identifier of the message.
     * @return An optional containing a mutable shared pointer to the message if found; std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<Message::SharedPtr> get_mut_by_uid(uint64_t message_snowflake);

    /**
     * @brief Adds a new message to the table.
     *
     * Creates and stores a new message with the given sender, channel, and content. On success, returns a
     * shared pointer to the newly created message; on failure, returns an error message string.
     *
     * @param sender_uid The UUID of the sender.
     * @param channel_uid The UUID of the channel.
     * @param content The content of the message.
     * @return A variant containing either a shared pointer to the new message on success or an error message string on failure.
     */
    std::variant<Message::SharedPtr, std::string> add_message(UUID sender_uid,
                                                              UUID channel_uid,
                                                              std::string content);

    /**
     * @brief Removes a message from the table.
     *
     * Deletes the message identified by the given snowflake identifier from the table.
     * On success, returns std::monostate; on failure, returns an error message string.
     *
     * @param message_snowflake The unique snowflake identifier of the message to remove.
     * @return A variant containing std::monostate on success or an error message string on failure.
     */
    std::variant<std::monostate, std::string> remove_message(uint64_t message_snowflake);

   private:
    /// Maps message snowflake identifiers to their corresponding shared pointers.
    std::unordered_map<uint64_t, Message::SharedPtr> data;
    /// Mutex to ensure thread-safe access to the message table.
    std::mutex mutex;
};
