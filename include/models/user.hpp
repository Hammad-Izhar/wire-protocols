#pragma once
#include <stdint.h>
#include <QObject>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "message/serialize.hpp"
#include "models/channel.hpp"
#include "models/message.hpp"
#include "models/uuid.hpp"

/**
 * @brief Represents a user in the system.
 *
 * The User class encapsulates user-related information such as username, display name, profile picture,
 * public key, and associated channels. It supports serialization to/from byte buffers and JSON, and integrates
 * with Qt's signal-slot mechanism to notify about events such as channel addition/removal and message reception/deletion.
 */
class User : public QObject, public Serializable {
    Q_OBJECT
   public:
    /**
     * @brief Shared pointer type for User.
     */
    typedef std::shared_ptr<User> SharedPtr;

    /**
     * @brief Constructs a new User with the specified username and display name.
     *
     * @param username The username of the user.
     * @param display_name The display name of the user.
     */
    User(std::string username, std::string display_name);

    /**
     * @brief Constructs a new User with the specified details.
     *
     * @param username The username of the user.
     * @param display_name The display name of the user.
     * @param uid The unique identifier for the user.
     * @param profile_pic The URL to the user's profile picture.
     */
    User(std::string username, std::string display_name, UUID uid, std::string profile_pic);

    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the User class with default values.
     */
    User() = default;

    /**
     * @brief Serializes the User object into a byte buffer.
     *
     * Converts the current state of the User into a sequence of bytes and appends it to the provided buffer.
     *
     * @param buf The byte buffer where the serialized data will be appended.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Deserializes the User object from a byte buffer.
     *
     * Restores the User's state from the provided byte buffer.
     *
     * @param buf The byte buffer containing the serialized user data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Converts the User object to a JSON string.
     *
     * Generates a JSON representation of the User's current state.
     *
     * @return A JSON string representing the User.
     */
    std::string to_json() const;

    /**
     * @brief Updates the User's state from a JSON string.
     *
     * Parses the provided JSON string and updates the User's fields accordingly.
     *
     * @param json A JSON string representing the User.
     */
    void from_json(const std::string& json);

    /**
     * @brief Gets the size of the serialized User object.
     *
     * Calculates and returns the number of bytes required to serialize the User.
     *
     * @return The size in bytes of the serialized User.
     */
    size_t size() const override;

    // Getters

    /**
     * @brief Retrieves the user's unique identifier.
     *
     * @return A constant reference to the user's UUID.
     */
    [[nodiscard]] const UUID& get_uid();

    /**
     * @brief Retrieves the user's username.
     *
     * @return A constant reference to the username.
     */
    [[nodiscard]] const std::string& get_username();

    /**
     * @brief Retrieves the user's display name.
     *
     * @return A constant reference to the display name.
     */
    [[nodiscard]] const std::string& get_display_name();

    /**
     * @brief Retrieves the user's profile picture URL.
     *
     * @return A constant reference to the profile picture URL.
     */
    [[nodiscard]] const std::string& get_profile_pic();

    /**
     * @brief Retrieves the list of channel IDs associated with the user.
     *
     * @return A constant reference to a vector of UUIDs representing the user's channels.
     */
    [[nodiscard]] const std::vector<UUID>& get_channels();

    // Setters

    /**
     * @brief Sets the user's username.
     *
     * @param username The new username for the user.
     */
    void set_username(std::string username);

    /**
     * @brief Sets the user's display name.
     *
     * @param display_name The new display name for the user.
     */
    void set_display_name(std::string display_name);

    /**
     * @brief Sets the user's profile picture URL.
     *
     * @param profile_pic The new profile picture URL.
     */
    void set_profile_pic(std::string profile_pic);

    /**
     * @brief Adds a channel to the user's list of channels.
     *
     * @param channel The UUID of the channel to add.
     */
    void add_channel(UUID channel);

    /**
     * @brief Removes a channel from the user's list of channels.
     *
     * @param channel_id The UUID of the channel to remove.
     */
    void remove_channel(UUID channel_id);

   signals:
    /**
     * @brief Signal emitted when a channel is added to the user.
     *
     * @param channel A shared pointer to the Channel that was added.
     */
    void channel_added(Channel::SharedPtr channel);

    /**
     * @brief Signal emitted when a channel is removed from the user.
     *
     * @param channel A shared pointer to the Channel that was removed.
     */
    void channel_removed(Channel::SharedPtr channel);

    /**
     * @brief Signal emitted when a message is received.
     *
     * @param message A shared pointer to the received Message.
     */
    void message_received(Message::SharedPtr message);

    /**
     * @brief Signal emitted when a message is deleted.
     *
     * @param message A shared pointer to the deleted Message.
     */
    void message_deleted(Message::SharedPtr message);

   private:
    /// The unique identifier for the user.
    UUID uid;
    /// The user's username.
    std::string username;
    /// The user's display name.
    std::string display_name;
    /// The URL to the user's profile picture.
    std::string profile_pic;
    /// The public key used for encryption.
    std::string public_key;
    /// A vector of channel UUIDs associated with the user.
    std::vector<UUID> channels;
    /// Mutex to ensure thread-safe access to the user's data.
    std::mutex mutex;
};
