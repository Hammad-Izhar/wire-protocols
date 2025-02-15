#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"

/**
 * @class ListAccountsMessage
 * @brief Represents a request message to list user accounts matching a regex pattern.
 *
 * This class is used to construct and process a request for listing user accounts.
 * It allows specifying a regular expression to filter the accounts and supports 
 * serialization and deserialization.
 */
class ListAccountsMessage : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    ListAccountsMessage() = default;

    /**
     * @brief Constructs a message with a specified regex pattern for filtering accounts.
     * @param regex A string representing the regex pattern to filter user accounts.
     */
    ListAccountsMessage(std::string regex);

    /**
     * @brief Serializes the message into a byte buffer.
     * @param buf The vector to store the serialized data.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes both the message and its header into a byte buffer.
     *
     * This function serializes the header and message content together
     * into a single byte buffer for transmission.
     *
     * @param buf The vector to store the serialized header and message data.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the message from a byte buffer.
     * @param buf The vector containing the serialized data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Converts the message to a JSON string representation.
     * @return A JSON string representing the message.
     */
    [[nodiscard]] std::string to_json() const;

    /**
     * @brief Populates the message from a JSON string.
     * @param json The JSON string to deserialize.
     */
    void from_json(const std::string& json);

    /**
     * @brief Retrieves the regex pattern used for filtering accounts.
     * @return A string representing the regex pattern.
     */
    [[nodiscard]] std::string get_regex() const;

    /**
     * @brief Retrieves the size of the serialized message.
     * @return The size of the serialized message in bytes.
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Sets a new regex pattern for filtering accounts.
     * @param regex A string representing the new regex pattern.
     */
    void set_regex(std::string regex);

   private:
   /**
     * @brief The regex pattern used to filter user accounts.
     */
    std::string regex;
};