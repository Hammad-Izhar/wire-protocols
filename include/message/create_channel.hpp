#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

/**
 * @class CreateChannelMessage
 * @brief Represents a message used to create a new channel.
 *
 * This class encapsulates the data required to create a new channel, including 
 * the channel name and a list of members. It supports serialization, deserialization, 
 * and JSON conversion.
 */
class CreateChannelMessage : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    CreateChannelMessage() = default;

    /**
     * @brief Constructs a CreateChannelMessage with a specified channel name and members.
     * @param channel_name The name of the channel to be created.
     * @param members A vector of UUIDs representing the members of the channel.
     */
    CreateChannelMessage(std::string channel_name, std::vector<UUID> members);

    /**
     * @brief Serializes the message into a byte buffer.
     * @param buf The vector to store the serialized data.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes the message and header data into a byte buffer.
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
     * @brief Gets the size of the serialized message.
     * @return The size of the serialized message in bytes.
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Retrieves the channel name.
     * @return A constant reference to the channel name.
     */
    [[nodiscard]] const std::string& get_channel_name() const;

    /**
     * @brief Retrieves the list of members in the channel.
     * @return A constant reference to the vector of member UUIDs.
     */
    [[nodiscard]] const std::vector<UUID>& get_members() const;

    /**
     * @brief Sets the channel name.
     * @param channel_name The new channel name.
     */
    void set_channel_name(std::string channel_name);

    /**
     * @brief Sets the members of the channel.
     * @param members A vector of UUIDs representing the new members.
     */
    void set_members(std::vector<UUID> members);

   private:
   /**
     * @brief The name of the channel to be created.
     */
    std::string channel_name;

    /**
     * @brief A vector of UUIDs representing the members of the channel.
     */
    std::vector<UUID> members;
};