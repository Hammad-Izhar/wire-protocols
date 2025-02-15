#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"

/**
 * @class DeleteAccountMessage
 * @brief Represents a request message to delete a user account.
 *
 * This class encapsulates the necessary data for an account deletion request, 
 * including the username and password. It supports serialization, deserialization, 
 * and JSON conversion.
 */
class DeleteAccountMessage : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    DeleteAccountMessage() = default;

    /**
     * @brief Constructs a DeleteAccountMessage with a username and password.
     * @param username The username of the account to be deleted.
     * @param password The password associated with the account.
     */
    DeleteAccountMessage(std::string username, std::string password);

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
     * @brief Gets the size of the serialized message.
     * @return The size of the serialized message in bytes.
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Retrieves the username associated with the account deletion request.
     * @return A constant reference to the username.
     */
    [[nodiscard]] const std::string& get_username() const;

    /**
     * @brief Retrieves the password associated with the account deletion request.
     * @return A constant reference to the password.
     */
    [[nodiscard]] const std::string& get_password() const;

    /**
     * @brief Sets the username for the account deletion request.
     * @param username The new username.
     */
    void set_username(const std::string& username);

    /**
     * @brief Sets the password for the account deletion request.
     * @param password The new password.
     */
    void set_password(const std::string& password);

   private:
   /**
     * @brief The username of the account to be deleted.
     */
    std::string username;
    /**
     * @brief The password associated with the account.
     */
    std::string password;
};