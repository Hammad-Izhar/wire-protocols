#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/user.hpp"

/**
 * @class DeleteAccountResponse
 * @brief Represents the response to an account deletion request.
 *
 * This class encapsulates the result of an account deletion operation, which 
 * can either be successful (returning a User object) or unsuccessful (returning 
 * an error message). It supports serialization, deserialization, and JSON conversion.
 */
class DeleteAccountResponse : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    DeleteAccountResponse() = default;

    /**
     * @brief Constructs a DeleteAccountResponse with either a User object or an error message.
     * @param data A variant that holds either a User::SharedPtr (on success) or a string error message (on failure).
     */
    DeleteAccountResponse(std::variant<User::SharedPtr, std::string> data);

    /**
     * @brief Serializes the response into a byte buffer.
     * @param buf The vector to store the serialized data.
     */
    void serialize(std::vector<uint8_t>& buf) const;

    /**
     * @brief Serializes the header and message into a byte buffer.
     * @param buf The vector to store the serialized message data.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the response from a byte buffer.
     * @param buf The vector containing the serialized data.
     */
    void deserialize(const std::vector<uint8_t>& buf);

    /**
     * @brief Converts the response into a JSON string representation.
     * @return A JSON string representing the response.
     */
    [[nodiscard]] std::string to_json() const;

    /**
     * @brief Populates the response object from a JSON string.
     * @param json The JSON string containing the response data.
     */
    void from_json(const std::string& json);

    /**
     * @brief Gets the size of the serialized response.
     * @return The size of the serialized response in bytes.
     */
    [[nodiscard]] size_t size() const;

    /**
     * @brief Checks if the account deletion was successful.
     * @return True if the operation was successful, otherwise false.
     */
    [[nodiscard]] bool is_success() const;

    /**
     * @brief Retrieves the error message if the operation failed.
     * @return An optional string containing the error message, or std::nullopt if successful.
     */
    [[nodiscard]] std::optional<std::string> get_error_message() const;

    /**
     * @brief Retrieves the deleted user data if the operation was successful.
     * @return An optional User::SharedPtr containing the deleted user, or std::nullopt if unsuccessful.
     */
    [[nodiscard]] std::optional<User::SharedPtr> get_data() const;

   private:
   /**
     * @brief Stores either the deleted user's information or an error message.
     */
    std::variant<User::SharedPtr, std::string> data;
};