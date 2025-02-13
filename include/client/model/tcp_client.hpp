#pragma once
#include <QHostAddress>
#include <QTcpSocket>

#include "models/user.hpp"

class TcpClient : public QObject {
    Q_OBJECT

   public:
    explicit TcpClient(QObject* parent = nullptr);

    void connectToServer(const QString& host, quint16 port);

    void disconnectFromServer();

    void register_user(const std::string& username,
                       const std::string& displayName,
                       const std::string& password);

    void login_user(const std::string& username, const std::string& password);

    void search_accounts(const std::string& regex);

    void delete_account(const std::string& username, const std::string& password);

    [[nodiscard]] QAbstractSocket::SocketState getConnectionStatus() const;

   signals:
    void registrationSuccess();
    void registrationFailure(const QString& errorMessage);

    void loginSuccess();
    void loginFailure(const QString& errorMessage);

    void searchSuccess(const std::vector<User::SharedPtr>& accounts);
    void searchFailure(const QString& errorMessage);

    void deleteAccountSuccess();
    void deleteAccountFailure(const QString& errorMessage);

   private:
    QTcpSocket* socket;

   private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
};