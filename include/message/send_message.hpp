#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

/**
 * @brief Represents a message to be sent within a specific channel.
 *
 * This class encapsulates the data required to send a message, including the unique identifiers
 * of both the channel and the sender, as well as the message text. It supports serialization to
 * and from byte buffers as well as conversion to and from JSON.
 */
class SendMessageMessage : public Serializable {
   public:
    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the SendMessageMessage class with default values.
     */
    SendMessageMessage() = default;

    /**
     * @brief Constructs a new SendMessageMessage with specified details.
     *
     * @param channel_uid The unique identifier of the channel where the message is sent.
     * @param sender_uid The unique identifier of the sender.
     * @param text The text content of the message.
     */
    SendMessageMessage(UUID channel_uid, UUID sender_uid, std::string text);

    /**
     * @brief Serializes the object into a byte buffer.
     *
     * Serializes the current state of the object and appends the resulting bytes
     * to the provided buffer.
     *
     * @param buf The byte buffer where the serialized data will be appended.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes the message-specific data into a byte buffer.
     *
     * Serializes only the fields specific to the message (e.g., channel UID, sender UID, and text)
     * into the provided byte buffer.
     *
     * @param buf The byte buffer where the serialized message data will be appended.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the object from a byte buffer.
     *
     * Reads data from the provided byte buffer to restore the object's state.
     *
     * @param buf The byte buffer containing the serialized data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Converts the object to a JSON string.
     *
     * Generates a JSON representation of the current state of the object.
     *
     * @return A JSON string representing the object.
     */
    [[nodiscard]] std::string to_json() const;

    /**
     * @brief Updates the object's state from a JSON string.
     *
     * Parses the provided JSON string and updates the object's fields accordingly.
     *
     * @param json A JSON string representing the object.
     */
    void from_json(const std::string& json);

    /**
     * @brief Retrieves the text content of the message.
     *
     * @return A string containing the text of the message.
     */
    [[nodiscard]] std::string get_text() const;

    /**
     * @brief Retrieves the unique identifier of the sender.
     *
     * @return The UUID representing the sender.
     */
    [[nodiscard]] UUID get_sender_uid() const;

    /**
     * @brief Retrieves the unique identifier of the channel.
     *
     * @return The UUID representing the channel.
     */
    [[nodiscard]] UUID get_channel_uid() const;

    /**
     * @brief Gets the size of the serialized object.
     *
     * Calculates and returns the number of bytes that would be produced by serializing the object.
     *
     * @return The size in bytes of the serialized object.
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Sets a regular expression pattern.
     *
     * This function sets a regex pattern that may be used for processing or validating the message text.
     *
     * @param regex A string representing the regular expression pattern.
     */
    void set_regex(std::string regex);

   private:
    /// The unique identifier of the channel where the message is sent.
    UUID channel_uid;
    /// The unique identifier of the sender.
    UUID sender_uid;
    /// The text content of the message.
    std::string text;
};
