#pragma once
#include <stdint.h>
#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"

/**
 * @brief Represents the response to a register account request.
 *
 * This class encapsulates the result of a registration attempt. It indicates whether the 
 * registration was successful (when no error message is set) or failed (when an error message is present).
 * The class supports serialization to and from both a byte buffer and JSON format.
 */
class RegisterAccountResponse : public Serializable {
   public:
    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the RegisterAccountResponse class with no error message,
     * indicating a successful registration.
     */
    RegisterAccountResponse() = default;

    /**
     * @brief Constructs a new RegisterAccountResponse with an error message.
     *
     * @param error_message A variant containing either std::monostate (for success) or a std::string
     *                      representing the error message.
     */
    RegisterAccountResponse(std::variant<std::monostate, std::string> error_message);

    /**
     * @brief Serializes the object into a byte buffer.
     *
     * This method overrides the base class serialize method and writes the current state of the 
     * object into the provided buffer.
     *
     * @param buf The byte buffer where the serialized data will be appended.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes the message into a byte buffer.
     *
     * This function specifically serializes the message part of the object into the provided buffer.
     *
     * @param buf The byte buffer where the serialized message will be appended.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the object from a byte buffer.
     *
     * Reads the object's state from the provided buffer, restoring its previous state.
     *
     * @param buf The byte buffer containing the serialized object data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Converts the object to a JSON string.
     *
     * Generates a JSON representation of the current state of the object.
     *
     * @return A string containing the JSON representation.
     */
    [[nodiscard]] std::string to_json() const;

    /**
     * @brief Sets the object's state from a JSON string.
     *
     * Parses the provided JSON string and updates the object's internal state accordingly.
     *
     * @param json A string containing the JSON representation of a RegisterAccountResponse.
     */
    void from_json(const std::string& json);

    /**
     * @brief Gets the size of the serialized object.
     *
     * Calculates and returns the number of bytes that would be produced by serializing the object.
     *
     * @return The size in bytes of the serialized object.
     */
    [[nodiscard]] size_t size() const;

    /**
     * @brief Checks if the registration was successful.
     *
     * A registration is considered successful if there is no error message present.
     *
     * @return true if the registration was successful; false otherwise.
     */
    [[nodiscard]] bool is_success() const;

    /**
     * @brief Retrieves the error message.
     *
     * Returns an optional error message. If the registration was successful, the optional will not contain a value.
     *
     * @return An optional string containing the error message, if any.
     */
    [[nodiscard]] const std::optional<std::string> get_error_message() const;

   private:
    /// Holds either an empty value (std::monostate) for success or a string describing the error.
    std::variant<std::monostate, std::string> error_message;
};
