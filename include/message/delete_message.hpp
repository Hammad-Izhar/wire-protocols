#pragma once
#include <stdint.h>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

/**
 * @class DeleteMessageMessage
 * @brief Represents a request to delete a specific message in a channel.
 *
 * This class encapsulates the necessary data to request the deletion of a 
 * message, including the channel UUID and the unique message identifier (snowflake). 
 * It supports serialization, deserialization, and JSON conversion.
 */
class DeleteMessageMessage : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    DeleteMessageMessage() = default;

    /**
     * @brief Constructs a DeleteMessageMessage with a channel UUID and message identifier.
     * @param channel_uid The UUID of the channel containing the message.
     * @param message_snowflake The unique identifier (snowflake) of the message to be deleted.
     */
    DeleteMessageMessage(UUID channel_uid, uint64_t message_snowflake);

    /**
     * @brief Serializes the message into a byte buffer.
     * @param buf The vector to store the serialized data.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes only the message-specific data into a byte buffer.
     * @param buf The vector to store the serialized message data.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the message from a byte buffer.
     * @param buf The vector containing the serialized data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Converts the message into a JSON string representation.
     * @return A JSON string representing the message.
     */
    [[nodiscard]] std::string to_json() const;

    /**
     * @brief Populates the message object from a JSON string.
     * @param json The JSON string containing the message data.
     */
    void from_json(const std::string& json);

    /**
     * @brief Retrieves the unique identifier (snowflake) of the message to be deleted.
     * @return The message snowflake.
     */
    [[nodiscard]] uint64_t get_message_snowflake() const;

    /**
     * @brief Gets the size of the serialized message.
     * @return The size of the serialized message in bytes.
     */
    [[nodiscard]] size_t size() const override;

   private:
   /**
     * @brief The UUID of the channel containing the message.
     */
    UUID channel_uid;
    /**
     * @brief The unique identifier (snowflake) of the message to be deleted.
     */
    uint64_t message_snowflake;
};