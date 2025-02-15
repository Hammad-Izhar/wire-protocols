#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include "message/serialize.hpp"

/**
 * @class LoginMessage
 * @brief Represents a login message containing user credentials.
 *
 * This class encapsulates the username and password required for logging in.
 * It provides functionality to serialize/deserialize the data into both binary and JSON formats.
 */
class LoginMessage : public Serializable {
   public:
    /**
     * @brief Default constructor.
     *
     * Constructs an empty LoginMessage instance.
     */
    LoginMessage() = default;

    /**
     * @brief Constructs a LoginMessage with provided credentials.
     *
     * @param username The username for login.
     * @param password The password for login.
     */
    LoginMessage(std::string username, std::string password);

    /**
     * @brief Serializes the LoginMessage into a binary buffer.
     *
     * Converts the complete LoginMessage (including metadata if any) into a binary format
     * and appends it to the provided buffer.
     *
     * @param buf The buffer to which the serialized data is appended.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes the core message payload into a binary buffer.
     *
     * Serializes only the essential part of the LoginMessage (username and password)
     * into a binary format and appends it to the provided buffer.
     *
     * @param buf The buffer to which the serialized message payload is appended.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the LoginMessage from a binary buffer.
     *
     * Reads the binary data from the provided buffer to reconstruct the LoginMessage object.
     *
     * @param buf The buffer containing the serialized LoginMessage.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Converts the LoginMessage to a JSON string.
     *
     * Serializes the login credentials into a JSON formatted string.
     *
     * @return A JSON string representing the LoginMessage.
     */
    [[nodiscard]] std::string to_json() const;

    /**
     * @brief Populates the LoginMessage from a JSON string.
     *
     * Parses the provided JSON string and updates the internal username and password
     * accordingly.
     *
     * @param json A JSON string representing the LoginMessage.
     */
    void from_json(const std::string& json);

    /**
     * @brief Retrieves the size in bytes of the serialized LoginMessage.
     *
     * Calculates the size of the LoginMessage when it is serialized into a binary format.
     *
     * @return The size in bytes of the serialized LoginMessage.
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Retrieves the username.
     *
     * @return A const reference to the username string.
     */
    [[nodiscard]] const std::string& get_username() const;

    /**
     * @brief Retrieves the password.
     *
     * @return A const reference to the password string.
     */
    [[nodiscard]] const std::string& get_password() const;

    /**
     * @brief Sets the username.
     *
     * Updates the username for this LoginMessage.
     *
     * @param username The new username.
     */
    void set_username(const std::string& username);

    /**
     * @brief Sets the password.
     *
     * Updates the password for this LoginMessage.
     *
     * @param password The new password.
     */
    void set_password(const std::string& password);

   private:
    std::string username; ///< The username used for login.
    std::string password; ///< The password used for login.
};
