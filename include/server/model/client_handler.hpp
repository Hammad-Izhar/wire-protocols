#pragma once
#include <QTcpSocket>
#include <optional>
#include <vector>
#include <variant>
#include <string>

#include "models/user.hpp"

/**
 * @brief Handles communication with a connected client.
 *
 * The ClientHandler class is responsible for managing a client's connection using a socket descriptor.
 * It uses Qt's signals and slots to asynchronously handle client events such as reading data,
 * writing data, disconnection, and message/channel events. The handler also maintains an optional
 * authenticated user associated with the client.
 */
class ClientHandler : public QObject {
    Q_OBJECT

   public:
    /**
     * @brief Constructs a new ClientHandler object.
     *
     * Initializes the client handler with the specified socket descriptor and an optional parent QObject.
     *
     * @param socket_descriptor The descriptor of the socket associated with the client.
     * @param parent Optional parent QObject.
     */
    explicit ClientHandler(qintptr socket_descriptor, QObject* parent = nullptr);

    /**
     * @brief Sets the authenticated user for the client.
     *
     * Associates the provided user with the client, indicating that the client has been successfully authenticated.
     *
     * @param user A shared pointer to the authenticated user.
     */
    void set_authenticated_user(const User::SharedPtr user);

   private:
    /// Pointer to the client's QTcpSocket.
    QTcpSocket* socket;
    /// The socket descriptor associated with the client.
    qintptr socket_descriptor;
    /// Optionally holds the authenticated user for this client.
    std::optional<User::SharedPtr> authenticated_user;

   public slots:
    /**
     * @brief Initiates handling of the client's connection.
     *
     * This slot is responsible for starting the event loop for the client, handling incoming data,
     * and coordinating the processing of client-related events.
     */
    void handle_client();

   private slots:
    /**
     * @brief Writes data to the client's socket.
     *
     * Called when there is data to be sent to the client.
     *
     * @param data A vector of bytes representing the data to be written.
     */
    void on_write_data(std::vector<uint8_t> data);

    /**
     * @brief Reads incoming data from the client's socket.
     *
     * Called when data is available on the socket. Processes the received data accordingly.
     */
    void on_read_data();

    /**
     * @brief Handles the client's disconnection event.
     *
     * Called when the client disconnects from the server.
     */
    void on_disconnected();

    /**
     * @brief Handles an incoming message event.
     *
     * Called when a message is received from the client. The message can either be a valid message
     * pointer or an error string.
     *
     * @param message A variant containing either a Message::SharedPtr or an error message string.
     */
    void on_message_received(std::variant<Message::SharedPtr, std::string> message);

    /**
     * @brief Handles a message deletion event.
     *
     * Called when a message deletion event occurs. The message parameter contains either a valid message
     * pointer or an error message string.
     *
     * @param message A variant containing either a Message::SharedPtr or an error message string.
     */
    void on_message_deleted(std::variant<Message::SharedPtr, std::string> message);

    /**
     * @brief Handles the event when a channel is added.
     *
     * Called when a new channel is added and relevant to the client. The channel parameter contains either a valid channel
     * pointer or an error message string.
     *
     * @param channel A variant containing either a Channel::SharedPtr or an error message string.
     */
    void on_channel_added(std::variant<Channel::SharedPtr, std::string> channel);

    // Note: The on_channel_removed slot is commented out in the current implementation.
    // void on_channel_removed(std::variant<Channel::SharedPtr, std::string> channel);

   signals:
    /**
     * @brief Signal emitted when the client handler has finished processing.
     *
     * This signal indicates that the client handler has completed its tasks and is ready for cleanup.
     */
    void finished();
};
