#pragma once
#include <stdint.h>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

/**
 * @brief Represents a communication channel.
 *
 * The Channel class encapsulates information about a communication channel, including its unique identifier,
 * name, associated user IDs, and message identifiers (snowflakes). It provides functionality for serialization,
 * JSON conversion, and thread-safe updates.
 */
class Channel : public Serializable {
   public:
    /**
     * @brief Shared pointer type for Channel.
     */
    typedef std::shared_ptr<Channel> SharedPtr;

    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the Channel class with default values.
     */
    Channel() = default;

    /**
     * @brief Constructs a new Channel with the specified name and users.
     *
     * @param name The name of the channel.
     * @param user_uids A vector of UUIDs representing the users associated with the channel.
     */
    Channel(std::string name, std::vector<UUID> user_uids);

    /**
     * @brief Serializes the Channel object into a byte buffer.
     *
     * Converts the current state of the Channel into a sequence of bytes and appends the data to the provided buffer.
     *
     * @param buf The byte buffer where the serialized data will be appended.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Deserializes the Channel object from a byte buffer.
     *
     * Restores the state of the Channel from the provided byte buffer.
     *
     * @param buf The byte buffer containing the serialized channel data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Converts the Channel object to a JSON string.
     *
     * Generates a JSON representation of the Channel's state.
     *
     * @return A JSON string representing the Channel.
     */
    std::string to_json() const;

    /**
     * @brief Updates the Channel's state from a JSON string.
     *
     * Parses the provided JSON string and updates the Channel's fields accordingly.
     *
     * @param json A JSON string representing the Channel.
     */
    void from_json(const std::string& json);

    /**
     * @brief Gets the size of the serialized Channel object.
     *
     * Calculates and returns the number of bytes required to serialize the Channel.
     *
     * @return The size in bytes of the serialized Channel.
     */
    [[nodiscard]] size_t size() const override;

    // Getters
    /**
     * @brief Retrieves the unique identifier (UUID) of the channel.
     *
     * @return A constant reference to the channel's UUID.
     */
    [[nodiscard]] const UUID& get_uid();

    /**
     * @brief Retrieves the name of the channel.
     *
     * @return A constant reference to the channel's name.
     */
    [[nodiscard]] const std::string& get_name();

    /**
     * @brief Retrieves the list of user UUIDs associated with the channel.
     *
     * @return A constant reference to the vector of user UUIDs.
     */
    [[nodiscard]] const std::vector<UUID>& get_user_uids();

    /**
     * @brief Retrieves the list of message identifiers (snowflakes) associated with the channel.
     *
     * @return A constant reference to the vector of message snowflakes.
     */
    [[nodiscard]] const std::vector<uint64_t>& get_message_snowflakes();

    // Setters
    /**
     * @brief Sets the name of the channel.
     *
     * @param name The new name for the channel.
     */
    void set_name(std::string name);

    /**
     * @brief Adds a user to the channel.
     *
     * @param user_uid The UUID of the user to be added.
     */
    void add_user(UUID user_uid);

    /**
     * @brief Adds a message identifier (snowflake) to the channel.
     *
     * @param message_snowflake The message snowflake to add.
     */
    void add_message(const uint64_t& message_snowflake);

    /**
     * @brief Removes a user from the channel.
     *
     * @param user_uid The UUID of the user to be removed.
     */
    void remove_user(const UUID& user_uid);

    /**
     * @brief Removes a message identifier (snowflake) from the channel.
     *
     * @param message_snowflake The message snowflake to remove.
     */
    void remove_message(const uint64_t& message_snowflake);

   private:
    /// The unique identifier for the channel.
    UUID uid;
    /// The name of the channel.
    std::string name;
    /// A vector of UUIDs representing the users associated with the channel.
    std::vector<UUID> user_uids;
    /// A vector of message identifiers (snowflakes) associated with the channel.
    std::vector<uint64_t> message_snowflakes;
    /// Mutex for thread-safe access and modification of channel data.
    std::mutex mutex;
};
