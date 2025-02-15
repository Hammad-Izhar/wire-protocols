#pragma once
#include <stdint.h>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

/**
 * @brief Represents a message with content and associated metadata.
 *
 * The Message class encapsulates all relevant data associated with a message in the system.
 * It includes information such as the sender, the channel, timestamps for creation and modification,
 * a unique snowflake identifier, a list of users who have read the message, and the message text.
 * The class supports serialization to and from byte buffers as well as conversion to and from JSON.
 */
class Message : public Serializable {
   public:
    /**
     * @brief Shared pointer type for Message.
     */
    typedef std::shared_ptr<Message> SharedPtr;

    /**
     * @brief Constructs a new Message with the specified sender, channel, and text.
     *
     * Initializes a new instance of the Message class with the provided sender identifier,
     * channel identifier, and text content. Other metadata (such as the snowflake, creation time,
     * and modification time) are set internally.
     *
     * @param sender_id The UUID of the sender.
     * @param channel_id The UUID of the channel.
     * @param text The text content of the message.
     */
    Message(UUID sender_id, UUID channel_id, std::string text);

    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the Message class with default values.
     */
    Message() = default;

    /**
     * @brief Serializes the Message object into a byte buffer.
     *
     * Converts the current state of the Message into a sequence of bytes and appends the data
     * to the provided buffer.
     *
     * @param buf The byte buffer where the serialized data will be appended.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes the message-specific fields into a byte buffer.
     *
     * Converts only the message-specific part of the Message (excluding additional metadata)
     * into a sequence of bytes and appends them to the provided buffer.
     *
     * @param buf The byte buffer where the serialized message data will be appended.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the Message object from a byte buffer.
     *
     * Reads data from the provided byte buffer and restores the state of the Message object.
     *
     * @param buf The byte buffer containing the serialized message data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Gets the size of the serialized Message object.
     *
     * Calculates and returns the number of bytes required to serialize the Message.
     *
     * @return The size in bytes of the serialized Message.
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Converts the Message object to a JSON string.
     *
     * Generates a JSON representation of the current state of the Message.
     *
     * @return A JSON string representing the Message.
     */
    std::string to_json() const;

    /**
     * @brief Updates the Message's state from a JSON string.
     *
     * Parses the provided JSON string and updates the Message's fields accordingly.
     *
     * @param json A JSON string representing the Message.
     */
    void from_json(const std::string& json);

    // Getters

    /**
     * @brief Retrieves the unique snowflake identifier of the Message.
     *
     * @return The snowflake identifier as a uint64_t.
     */
    [[nodiscard]] const uint64_t get_snowflake();

    /**
     * @brief Retrieves the sender's unique identifier.
     *
     * @return A constant reference to the sender's UUID.
     */
    [[nodiscard]] const UUID& get_sender_id();

    /**
     * @brief Retrieves the channel's unique identifier.
     *
     * @return A constant reference to the channel's UUID.
     */
    [[nodiscard]] const UUID& get_channel_id();

    /**
     * @brief Retrieves the creation timestamp of the Message.
     *
     * @return The creation timestamp as a uint64_t.
     */
    [[nodiscard]] const uint64_t get_created_at();

    /**
     * @brief Retrieves the modification timestamp of the Message.
     *
     * @return The modification timestamp as a uint64_t.
     */
    [[nodiscard]] const uint64_t get_modified_at();

    /**
     * @brief Retrieves the list of user UUIDs who have read the Message.
     *
     * @return A constant reference to the vector containing the UUIDs of users who have read the Message.
     */
    [[nodiscard]] const std::vector<UUID>& get_read_by();

    /**
     * @brief Retrieves the text content of the Message.
     *
     * @return A constant reference to the text content of the Message.
     */
    [[nodiscard]] const std::string& get_text();

    // Setters

    /**
     * @brief Updates the text content of the Message.
     *
     * Sets the text content of the Message to the provided value.
     *
     * @param text A reference to a string containing the new text content.
     */
    void set_text(std::string& text);

    /**
     * @brief Adds a user to the list of users who have read the Message.
     *
     * Appends the provided user UUID to the list of readers.
     *
     * @param user_id A reference to the UUID of the user who has read the Message.
     */
    void set_read_by(UUID& user_id);

   private:
    /// A unique identifier for the Message (commonly referred to as a snowflake).
    uint64_t snowflake;
    /// The unique identifier of the sender.
    UUID sender_id;
    /// The unique identifier of the channel to which the Message belongs.
    UUID channel_id;
    /// The timestamp when the Message was created.
    uint64_t created_at;
    /// The timestamp when the Message was last modified.
    uint64_t modified_at;
    /// A vector of UUIDs representing users who have read the Message.
    std::vector<UUID> read_by;
    /// The text content of the Message.
    std::string text;
    /// Mutex to ensure thread-safe access to the Message's data.
    std::mutex mutex;
};
