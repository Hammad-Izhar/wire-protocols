#pragma once
#include <QTcpSocket>
#include <QHostAddress>

class TcpClient : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject *parent = nullptr);

    void connectToServer(const QString &host, quint16 port);

    void disconnectFromServer();

    [[nodiscard]] QAbstractSocket::SocketState getConnectionStatus() const;

private:
    QTcpSocket *socket;

private slots:
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);
};