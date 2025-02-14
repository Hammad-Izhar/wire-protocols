#pragma once
#include <QHostAddress>
#include <QTcpSocket>

#include "models/channel.hpp"
#include "models/message.hpp"
#include "models/user.hpp"
#include "models/uuid.hpp"

class TcpClient : public QObject {
    Q_OBJECT

   public:
    explicit TcpClient(QObject* parent = nullptr);

    void connectToServer(const QString& host, quint16 port);

    void disconnectFromServer();

    void register_user(const std::string& username,
                       const std::string& display_name,
                       const std::string& password);

    void login_user(const std::string& username, const std::string& password);

    void search_accounts(const std::string& regex);

    void delete_account(const std::string& username, const std::string& password);

    void create_channel(const std::string& channel_name, const std::vector<UUID>& members);

    void send_text_message(const UUID& channel_uid,
                           const UUID& sender_uid,
                           const std::string& text);

    [[nodiscard]] QAbstractSocket::SocketState getConnectionStatus() const;

   signals:
    void registrationSuccess();
    void registrationFailure(const QString& error_message);

    void loginSuccess();
    void loginFailure(const QString& error_message);

    void searchSuccess(const std::vector<User::SharedPtr>& accounts);
    void searchFailure(const QString& error_message);

    void deleteAccountSuccess();
    void deleteAccountFailure(const QString& error_message);

    void createChannelSuccess(Channel::SharedPtr channel);
    void createChannelFailure(const QString& error_message);

    void sendMessageSuccess(Message::SharedPtr message);
    void sendMessageFailure(const QString& error_message);

   private:
    QTcpSocket* socket;

   private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
};