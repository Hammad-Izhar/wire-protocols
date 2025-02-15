#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/message.hpp"

/**
 * @class DeleteMessageResponse
 * @brief Represents the response to a message deletion request.
 *
 * This class encapsulates the result of a message deletion operation, which 
 * can either be successful (returning a Message object) or unsuccessful 
 * (returning an error message). It supports serialization, deserialization, 
 * and JSON conversion.
 */
class DeleteMessageResponse : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    DeleteMessageResponse() = default;

    /**
     * @brief Constructs a DeleteMessageResponse with either a Message object or an error message.
     * @param data A variant that holds either a Message::SharedPtr (on success) or a string error message (on failure).
     */
    DeleteMessageResponse(std::variant<Message::SharedPtr, std::string> data);

    /**
     * @brief Serializes the response into a byte buffer.
     * @param buf The vector to store the serialized data.
     */
    void serialize(std::vector<uint8_t>& buf) const;

    /**
     * @brief Serializes only the response-specific data into a byte buffer.
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
     * @brief Checks if the message deletion was successful.
     * @return True if the operation was successful, otherwise false.
     */
    [[nodiscard]] bool is_success() const;

    /**
     * @brief Retrieves the error message if the operation failed.
     * @return An optional string containing the error message, or std::nullopt if successful.
     */
    [[nodiscard]] std::optional<std::string> get_error_message() const;

    /**
     * @brief Retrieves the deleted message data if the operation was successful.
     * @return An optional Message::SharedPtr containing the deleted message, or std::nullopt if unsuccessful.
     */
    [[nodiscard]] std::optional<Message::SharedPtr> get_data() const;

   private:
   /**
     * @brief Stores either the deleted message's information or an error message.
     */
    std::variant<Message::SharedPtr, std::string> data;
};