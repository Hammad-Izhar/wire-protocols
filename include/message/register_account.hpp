#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include "message/serialize.hpp"

/**
 * @brief Represents a registration account message.
 *
 * This class encapsulates the information required to register a new account.
 * It includes methods for serialization, deserialization, and conversion to/from JSON.
 */
class RegisterAccountMessage : public Serializable {
   public:
    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the RegisterAccountMessage class with empty fields.
     */
    RegisterAccountMessage() = default;

    /**
     * @brief Constructs a new RegisterAccountMessage with specified user details.
     *
     * @param username The username for the new account.
     * @param password The password for the new account.
     * @param display_name The display name for the new account.
     */
    RegisterAccountMessage(std::string username, std::string password, std::string display_name);

    /**
     * @brief Serializes the object into a byte buffer.
     *
     * Converts the current state of the object into a sequence of bytes and appends them to the provided buffer.
     *
     * @param buf The byte buffer where the serialized data will be appended.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes the message-specific data into a byte buffer.
     *
     * Converts the registration fields into a sequence of bytes and appends them to the provided buffer.
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
     * @brief Gets the size of the serialized object.
     *
     * Calculates and returns the number of bytes that would be produced by serializing the object.
     *
     * @return The size in bytes of the serialized object.
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Retrieves the username.
     *
     * @return A constant reference to the username string.
     */
    [[nodiscard]] const std::string& get_username() const;

    /**
     * @brief Retrieves the password.
     *
     * @return A constant reference to the password string.
     */
    [[nodiscard]] const std::string& get_password() const;

    /**
     * @brief Retrieves the display name.
     *
     * @return A constant reference to the display name string.
     */
    [[nodiscard]] const std::string& get_display_name() const;

    /**
     * @brief Sets the username.
     *
     * Updates the username field with the provided value.
     *
     * @param username The new username.
     */
    void set_username(std::string username);

    /**
     * @brief Sets the password.
     *
     * Updates the password field with the provided value.
     *
     * @param password The new password.
     */
    void set_password(std::string password);

    /**
     * @brief Sets the display name.
     *
     * Updates the display name field with the provided value.
     *
     * @param display_name The new display name.
     */
    void set_display_name(std::string display_name);

   private:
    /// The username for the new account.
    std::string username;
    /// The password for the new account.
    std::string password;
    /// The display name for the new account.
    std::string display_name;
};
