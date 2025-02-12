#pragma once
#include <QHostAddress>
#include <QTcpSocket>

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

    [[nodiscard]] QAbstractSocket::SocketState getConnectionStatus() const;

   signals:
    void registrationSuccess();
    void registrationFailure(const QString& errorMessage);

    void loginSuccess();
    void loginFailure(const QString& errorMessage);

   private:
    QTcpSocket* socket;

   private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
    void onReadyRead();
};