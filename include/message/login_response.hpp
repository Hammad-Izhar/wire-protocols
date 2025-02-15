#pragma once
#include <stdint.h>
#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/user.hpp"

/**
 * @class LoginResponse
 * @brief Represents the response of a login operation.
 *
 * This class encapsulates the result of a login attempt, which can either be a valid user object
 * (on success) or an error message string (on failure). It implements the Serializable interface
 * for converting the response to/from binary and JSON formats.
 */
class LoginResponse : public Serializable {
    public:
        /**
         * @brief Default constructor.
         *
         * Constructs an empty LoginResponse instance.
         */
        LoginResponse() = default;
    
        /**
         * @brief Constructs a LoginResponse with the provided data.
         *
         * @param data A variant holding either a successful User pointer or an error message string.
         */
        LoginResponse(std::variant<User::SharedPtr, std::string> data);
    
        /**
         * @brief Serializes the LoginResponse into a binary buffer.
         *
         * Converts the internal state of the LoginResponse into a binary representation and appends it to
         * the provided buffer.
         *
         * @param buf The buffer to which the serialized data is appended.
         */
        void serialize(std::vector<uint8_t>& buf) const override;
    
        /**
         * @brief Serializes the core message payload into a binary buffer.
         *
         * Serializes the essential message data of the LoginResponse into a binary representation and
         * appends it to the provided buffer. This method may omit some metadata compared to the full
         * serialization performed by serialize().
         *
         * @param buf The buffer to which the serialized message is appended.
         */
        void serialize_msg(std::vector<uint8_t>& buf) const;
    
        /**
         * @brief Deserializes the LoginResponse from a binary buffer.
         *
         * Reads the binary representation from the provided buffer to reconstruct the state of the
         * LoginResponse.
         *
         * @param buf The binary buffer containing the serialized LoginResponse.
         */
        void deserialize(const std::vector<uint8_t>& buf) override;
    
        /**
         * @brief Converts the LoginResponse to a JSON string.
         *
         * Serializes the LoginResponse's data into a JSON formatted string.
         *
         * @return A JSON string representing the LoginResponse.
         */
        [[nodiscard]] std::string to_json() const;
    
        /**
         * @brief Populates the LoginResponse from a JSON string.
         *
         * Parses the provided JSON string and updates the LoginResponse's state accordingly.
         *
         * @param json A JSON string representing the LoginResponse.
         */
        void from_json(const std::string& json);
    
        /**
         * @brief Retrieves the size of the serialized LoginResponse.
         *
         * Computes and returns the size in bytes of the LoginResponse when it is serialized.
         *
         * @return The size in bytes of the serialized LoginResponse.
         */
        [[nodiscard]] size_t size() const;
    
        /**
         * @brief Indicates whether the login attempt was successful.
         *
         * @return true if the login attempt resulted in a valid user object; false if it failed.
         */
        [[nodiscard]] bool is_success() const;
    
        /**
         * @brief Retrieves the user data if the login was successful.
         *
         * @return An optional containing the User pointer if the login was successful, or an empty optional otherwise.
         */
        [[nodiscard]] const std::optional<User::SharedPtr> get_data() const;
    
        /**
         * @brief Retrieves the error message if the login attempt failed.
         *
         * @return An optional containing the error message string if the login failed, or an empty optional otherwise.
         */
        [[nodiscard]] const std::optional<std::string> get_error_message() const;
    
    private:
        /// Holds either the successful user data (User::SharedPtr) or an error message (std::string)
        std::variant<User::SharedPtr, std::string> data;
    };
    