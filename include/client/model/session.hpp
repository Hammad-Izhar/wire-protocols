#pragma once
#include <QObject>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "client/gui/components/stacked_window.hpp"
#include "client/model/tcp_client.hpp"
#include "models/uuid.hpp"

enum Window { CONNECTION = 0, AUTHENTICATION = 1, MAIN = 2 };

/**
 * @class Session
 * @brief Manages the application's session, including user authentication, channels, and messages.
 *
 * The Session class follows the singleton pattern and maintains the current application's state,
 * including the active user, active channel, and network client.
 */
class Session : public QObject {
    Q_OBJECT

   public:
   /**
     * @brief Stores the connection address (IP and port) if available.
     */
    std::optional<std::pair<std::string, uint16_t>> connection_address;

    /**
     * @brief Stores the authenticated user if they are logged in.
     */
    std::optional<User::SharedPtr> authenticated_user;

    /**
     * @brief Pointer to the TCP client managing network communication.
     */
    TcpClient* tcp_client;

    /**
     * @brief Pointer to the main application window stack.
     */
    StackedWindow* main_window;

    /**
     * @brief Retrieves the singleton instance of the Session class.
     * @return Reference to the singleton Session instance.
     */
    static Session& get_instance();

    /**
     * @brief Gets the currently active channel.
     * @return An optional shared pointer to the active Channel object.
     */
    std::optional<Channel::SharedPtr> get_active_channel() const;

    /**
     * @brief Gets the user ID of the currently active user.
     * @return An optional UUID representing the active user ID.
     */
    std::optional<UUID> get_active_user_id() const;

    /**
     * @brief Sets the active channel.
     * @param channel A shared pointer to the channel to be set as active.
     */
    void set_active_channel(const Channel::SharedPtr& channel);

    /**
     * @brief Adds a new channel to the session.
     * @param channel A shared pointer to the channel to be added.
     */
    void add_channel(const Channel::SharedPtr& channel);

    /**
     * @brief Adds a message to the active channel.
     * @param message A shared pointer to the message to be added.
     */
    void add_message(const Message::SharedPtr& message);

    /**
     * @brief Removes a message from the active channel.
     * @param message A shared pointer to the message to be removed.
     */
    void remove_message(const Message::SharedPtr& message);

    /**
     * @brief Resets the session, clearing user authentication and active channels.
     */
    void reset();

    /**
     * @brief Retrieves messages from the active channel.
     * @return A reference to a vector containing shared pointers to messages.
     */
    const std::vector<Message::SharedPtr>& get_active_channel_messages() const;

   private:
    std::optional<Channel::SharedPtr> open_channel;
    std::unordered_map<UUID, Channel::SharedPtr> channels;
    std::unordered_map<UUID, std::vector<Message::SharedPtr>> channel_messages;

    /**
     * @brief Private constructor to enforce the singleton pattern.
     */
    Session() = default;

    /**
     * @brief Private destructor.
     */
    ~Session() = default;

    /**
     * @brief Deleted copy constructor to prevent duplication.
     */
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&) = delete;
    Session& operator=(Session&&) = delete;

   signals:
   /**
     * @brief Signal emitted when the active channel is updated.
     */
    void updateActiveChannel();
};