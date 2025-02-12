#pragma once
#include <QTcpSocket>
#include <optional>

#include "models/user.hpp"

class ClientHandler : public QObject
{
    Q_OBJECT

public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);

    void setAuthenticatedUser(const User::SharedPtr &user);

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;
    std::optional<User::SharedPtr> authenticated_user;

public slots:
    void handleClient();

private slots:
    void onReadyRead();
    void onDisconnected();

signals:
    void finished();
};