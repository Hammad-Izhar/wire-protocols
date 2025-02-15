#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "message/serialize.hpp"
#include "models/message.hpp"

/**
 * @brief Represents the response to a send message request.
 *
 * This class encapsulates the result of a send message operation. It contains either a
 * shared pointer to a Message (indicating a successful operation) or a string representing
 * an error message (indicating failure). The class provides functionality for serialization
 * and deserialization to/from byte buffers as well as conversion to/from JSON.
 */
class SendMessageResponse : public Serializable {
   public:
    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the SendMessageResponse class with no data.
     */
    SendMessageResponse() = default;

    /**
     * @brief Constructs a new SendMessageResponse with the provided data.
     *
     * @param data A variant containing either a shared pointer to a Message (on success)
     *             or a string error message (on failure).
     */
    SendMessageResponse(std::variant<Message::SharedPtr, std::string> data);

    /**
     * @brief Serializes the object into a byte buffer.
     *
     * Serializes the current state of the object and appends the resulting bytes
     * to the provided buffer.
     *
     * @param buf The byte buffer where the serialized data will be appended.
     */
    void serialize(std::vector<uint8_t>& buf) const;

    /**
     * @brief Serializes the message-specific data into a byte buffer.
     *
     * Serializes only the message-related portion of the object into the provided byte buffer.
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
    void deserialize(const std::vector<uint8_t>& buf);

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
    [[nodiscard]] size_t size() const;

    /**
     * @brief Determines if the send message operation was successful.
     *
     * A successful operation is indicated when the internal data holds a Message pointer.
     *
     * @return true if the data variant contains a Message (indicating success), false if it contains an error message.
     */
    [[nodiscard]] bool is_success() const;

    /**
     * @brief Retrieves the error message, if any.
     *
     * Returns an optional error message if the send message operation failed.
     *
     * @return An optional string containing the error message, or std::nullopt if the operation was successful.
     */
    [[nodiscard]] std::optional<std::string> get_error_message() const;

    /**
     * @brief Retrieves the message data.
     *
     * Returns an optional shared pointer to a Message if the send message operation was successful.
     *
     * @return An optional Message::SharedPtr containing the message data, or std::nullopt if an error occurred.
     */
    [[nodiscard]] std::optional<Message::SharedPtr> get_data() const;

   private:
    /// Holds either a shared pointer to a Message (on success) or a string error message (on failure).
    std::variant<Message::SharedPtr, std::string> data;
};
