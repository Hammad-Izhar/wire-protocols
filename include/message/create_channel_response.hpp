#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/channel.hpp"

/**
 * @class CreateChannelResponse
 * @brief Represents a response to a create channel request.
 *
 * This class encapsulates the response from a channel creation operation, which can either be 
 * successful (containing a Channel object) or failed (containing an error message).
 * It implements serialization and deserialization functionality.
 */
class CreateChannelResponse : public Serializable {
   public:
   /**
     * @brief Default constructor.
     */
    CreateChannelResponse() = default;

    /**
     * @brief Constructs a CreateChannelResponse object with the given data.
     * @param data A variant containing either a shared pointer to a Channel (on success) 
     *             or an error message string (on failure).
     */
    CreateChannelResponse(std::variant<Channel::SharedPtr, std::string> data);

    /**
     * @brief Serializes the response object into a byte buffer.
     * @param buf The vector to store the serialized data.
     */
    void serialize(std::vector<uint8_t>& buf) const;

    /**
     * @brief Serializes only the message portion of the response.
     * @param buf The vector to store the serialized message data.
     */
    void serialize_msg(std::vector<uint8_t>& buf) const;

    /**
     * @brief Deserializes the response object from a byte buffer.
     * @param buf The vector containing the serialized data.
     */
    void deserialize(const std::vector<uint8_t>& buf);

    /**
     * @brief Converts the response to a JSON string representation.
     * @return A JSON string representing the response.
     */
    [[nodiscard]] std::string to_json() const;

    /**
     * @brief Populates the response object from a JSON string.
     * @param json The JSON string containing response data.
     */
    void from_json(const std::string& json);

    /**
     * @brief Gets the size of the serialized response object.
     * @return The size of the serialized response in bytes.
     */
    [[nodiscard]] size_t size() const;

    /**
     * @brief Checks whether the response indicates a successful channel creation.
     * @return True if the response contains a valid Channel, false if it contains an error message.
     */
    [[nodiscard]] bool is_success() const;

    /**
     * @brief Retrieves the error message if the response indicates failure.
     * @return An optional string containing the error message, or std::nullopt if successful.
     */
    [[nodiscard]] std::optional<std::string> get_error_message() const;

    /**
     * @brief Retrieves the created channel if the response is successful.
     * @return An optional shared pointer to the created Channel, or std::nullopt if failed.
     */
    [[nodiscard]] std::optional<Channel::SharedPtr> get_data();

   private:
   /**
     * @brief Stores the response data, which can be either a successful Channel object 
     *        or an error message string.
     */
    std::variant<Channel::SharedPtr, std::string> data;
};