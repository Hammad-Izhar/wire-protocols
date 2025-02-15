#pragma once
#include <stdint.h>
#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/user.hpp"

/**
 * @class ListAccountsResponse
 * @brief Represents a response to a request for listing user accounts.
 *
 * This class handles the response for a request to list user accounts. 
 * It stores either a list of user accounts on success or an error message 
 * on failure. The class supports serialization and deserialization.
 */
class ListAccountsResponse : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    ListAccountsResponse() = default;

    /**
     * @brief Constructs a successful response containing a list of user accounts.
     * @param data A vector of shared pointers to User objects.
     */
    ListAccountsResponse(std::vector<User::SharedPtr> data);

    /**
     * @brief Constructs a response that can contain either user accounts or an error message.
     * @param data A variant that holds either a list of user accounts or an error message.
     */
    ListAccountsResponse(std::variant<std::vector<User::SharedPtr>, std::string> data);

    /**
     * @brief Serializes the response into a byte buffer.
     * @param buf The vector to store the serialized data.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Serializes the message and the header together.
     * @param buf The vector to store the serialized message data.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the response from a byte buffer.
     * @param buf The vector containing the serialized data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Converts the response to a JSON string representation.
     * @return A JSON string representing the response.
     */
    [[nodiscard]] std::string to_json() const;

    /**
     * @brief Populates the response from a JSON string.
     * @param json The JSON string to deserialize.
     */
    void from_json(const std::string& json);

    /**
     * @brief Retrieves the size of the serialized response.
     * @return The size of the serialized response in bytes.
     */

    [[nodiscard]] size_t size() const override;

    /**
     * @brief Checks whether the response indicates a successful account retrieval.
     * @return True if the response contains user data, false if it contains an error message.
     */
    [[nodiscard]] bool is_success() const;

    /**
     * @brief Retrieves the error message if the response indicates a failure.
     * @return An optional string containing the error message, or std::nullopt if successful.
     */
    [[nodiscard]] std::optional<std::string> get_error_message() const;

    /**
     * @brief Retrieves the list of user accounts if the response is successful.
     * @return An optional vector of shared pointers to User objects, or std::nullopt if an error occurred.
     */
    [[nodiscard]] std::optional<std::vector<User::SharedPtr>> get_users();

   private:
   /**
     * @brief Holds either a list of user accounts on success or an error message on failure.
     */
    std::variant<std::vector<User::SharedPtr>, std::string> data;
};