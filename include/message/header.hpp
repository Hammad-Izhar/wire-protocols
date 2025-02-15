#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"

/**
 * @enum Operation
 * @brief Defines various operations that can be performed in the system.
 *
 * This enumeration represents different types of operations that a request
 * can perform, such as registering an account, sending a message, or 
 * creating a channel.
 */
enum Operation : uint8_t {
    REGISTER_ACCOUNT,
    LOGIN,
    LIST_ACCOUNTS,
    DELETE_ACCOUNT,
    SEND_MESSAGE,
    READ_MESSAGE,
    DELETE_MESSAGE,
    EDIT_MESSAGE,
    UNREAD_MESSAGE,
    CREATE_CHANNEL,
    UPDATE_CHANNEL_NAME,
    UPDATE_DISPLAY_NAME,
    UPDATE_PROFILE_PICTURE,
    RESET_PASSWORD,
};

/**
 * @class Header
 * @brief Represents the header of a serialized packet.
 *
 * This class encapsulates metadata for a packet, including its protocol 
 * version, the type of operation being performed, and the total length 
 * of the packet. It supports serialization and deserialization.
 */
class Header : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    Header() = default;

    /**
     * @brief Constructs a Header with the given version, operation, and packet length.
     * @param version The protocol version.
     * @param operation The operation type (e.g., LOGIN, SEND_MESSAGE).
     * @param packet_length The total length of the packet in bytes.
     */
    Header(uint8_t version, enum Operation operation, uint16_t packet_length);

    /**
     * @brief Serializes the header into a byte buffer.
     * @param buf The vector to store the serialized data.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Deserializes the header from a byte buffer.
     * @param buf The vector containing the serialized data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Retrieves the size of the serialized header.
     * @return The size of the serialized header in bytes.
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Gets the protocol version of the packet.
     * @return The protocol version.
     */
    [[nodiscard]] uint8_t get_version() const;

    /**
     * @brief Gets the operation type of the packet.
     * @return The operation type.
     */
    [[nodiscard]] enum Operation get_operation() const;

    /**
     * @brief Gets the total packet length.
     * @return The length of the packet in bytes.
     */
    [[nodiscard]] uint16_t get_packet_length() const;

    /**
     * @brief Sets the protocol version of the header.
     * @param version The protocol version.
     */
    void set_version(uint8_t version);

    /**
     * @brief Sets the operation type for the header.
     * @param operation The operation to be set.
     */
    void set_operation(enum Operation operation);

    /**
     * @brief Sets the packet length.
     * @param packet_length The total length of the packet in bytes.
     */
    void set_packet_length(uint16_t packet_length);

   private:
   /**
     * @brief The protocol version.
     */
    uint8_t version;

    /**
     * @brief The operation associated with this packet.
     */
    enum Operation operation;

    /**
     * @brief The total packet length in bytes.
     */
    uint16_t packet_length;
};