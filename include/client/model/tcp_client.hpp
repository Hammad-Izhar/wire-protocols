#pragma once
#include <QHostAddress>
#include <QTcpSocket>

#include "models/channel.hpp"
#include "models/message.hpp"
#include "models/user.hpp"
#include "models/uuid.hpp"

/**
 * @class TcpClient
 * @brief Manages network communication with the server via TCP.
 *
 * The TcpClient class handles user authentication, messaging, and channel management
 * by sending requests to and receiving responses from the server.
 */
class TcpClient : public QObject {
    Q_OBJECT

   public:
   /**
     * @brief Constructs a TcpClient object.
     * @param parent A pointer to the parent QObject (default is nullptr).
     */
    explicit TcpClient(QObject* parent = nullptr);

    /**
     * @brief Connects to the server at the specified host and port.
     * @param host The server hostname or IP address.
     * @param port The server port number.
     */
    void connectToServer(const QString& host, quint16 port);

    /**
     * @brief Disconnects from the server.
     */
    void disconnectFromServer();

    /**
     * @brief Registers a new user with the given credentials.
     * @param username The username for the new account.
     * @param display_name The display name for the new account.
     * @param password The password for the new account.
     */
    void register_user(const std::string& username,
                       const std::string& display_name,
                       const std::string& password);

    /**
     * @brief Logs in a user with the given credentials.
     * @param username The username of the account.
     * @param password The password of the account.
     */
    void login_user(const std::string& username, const std::string& password);

    /**
     * @brief Searches for user accounts matching a given regular expression.
     * @param regex The regular expression used to search for accounts.
     */
    void search_accounts(const std::string& regex);

    /**
     * @brief Deletes a user account.
     * @param username The username of the account to delete.
     * @param password The password for authentication.
     */
    void delete_account(const std::string& username, const std::string& password);

    /**
     * @brief Deletes a specific message.
     * @param message A shared pointer to the message to be deleted.
     */
    void delete_message(Message::SharedPtr message);

    /**
     * @brief Creates a new channel with the given members.
     * @param channel_name The name of the new channel.
     * @param members A vector of UUIDs representing the members of the channel.
     */
    void create_channel(const std::string& channel_name, const std::vector<UUID>& members);

    /**
     * @brief Sends a text message to a specific channel.
     * @param channel_uid The UUID of the target channel.
     * @param sender_uid The UUID of the sender.
     * @param text The message content.
     */
    void send_text_message(const UUID& channel_uid,
                           const UUID& sender_uid,
                           const std::string& text);

    /**
     * @brief Gets the current connection status of the socket.
     * @return The current socket state.
     */
    [[nodiscard]] QAbstractSocket::SocketState getConnectionStatus() const;

   signals:
    /**
     * @brief Emitted when user registration is successful.
     */
    void registrationSuccess();

    /**
     * @brief Emitted when user registration fails.
     * @param error_message The error message describing the failure reason.
     */
    void registrationFailure(const QString& error_message);

    /**
     * @brief Emitted when user login is successful.
     */
    void loginSuccess();

    /**
     * @brief Emitted when user login fails.
     * @param error_message The error message describing the failure reason.
     */
    void loginFailure(const QString& error_message);

    /**
     * @brief Emitted when a search operation succeeds.
     * @param accounts A list of user accounts matching the search query.
     */
    void searchSuccess(const std::vector<User::SharedPtr>& accounts);

    /**
     * @brief Emitted when a search operation fails.
     * @param error_message The error message describing the failure reason.
     */
    void searchFailure(const QString& error_message);

    /**
     * @brief Emitted when a user account is successfully deleted.
     */
    void deleteAccountSuccess();

    /**
     * @brief Emitted when deleting a user account fails.
     * @param error_message The error message describing the failure reason.
     */
    void deleteAccountFailure(const QString& error_message);

    /**
     * @brief Emitted when a channel is successfully created.
     * @param channel A shared pointer to the created channel.
     */
    void createChannelSuccess(Channel::SharedPtr channel);

    /**
     * @brief Emitted when creating a channel fails.
     * @param error_message The error message describing the failure reason.
     */
    void createChannelFailure(const QString& error_message);

    /**
     * @brief Emitted when a message is successfully sent.
     * @param message A shared pointer to the sent message.
     */
    void sendMessageSuccess(Message::SharedPtr message);

    /**
     * @brief Emitted when sending a message fails.
     * @param error_message The error message describing the failure reason.
     */
    void sendMessageFailure(const QString& error_message);

    /**
     * @brief Emitted when a message is successfully deleted.
     * @param message A shared pointer to the deleted message.
     */
    void deleteMessageSuccess(Message::SharedPtr message);

    /**
     * @brief Emitted when deleting a message fails.
     * @param error_message The error message describing the failure reason.
     */
    void deleteMessageFailure(const QString& error_message);

   private:
    QTcpSocket* socket; ///< The TCP socket used for network communication.

   private slots:
   /**
     * @brief Slot triggered when the client successfully connects to the server.
     */
    void onConnected();

    /**
     * @brief Slot triggered when the client disconnects from the server.
     */
    void onDisconnected();

    /**
     * @brief Slot triggered when a socket error occurs.
     * @param socketError The error code representing the socket error.
     */
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

    /**
     * @brief Slot triggered when data is available to read from the socket.
     */
    void onReadyRead();
};