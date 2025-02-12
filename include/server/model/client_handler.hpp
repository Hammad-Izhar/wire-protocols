#pragma once
#include <QTcpSocket>

class ClientHandler : public QObject
{
    Q_OBJECT

public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);

private:
    QTcpSocket *socket;
    qintptr socketDescriptor;

public slots:
    void handleClient();

private slots:
    void onReadyRead();
    void onDisconnected();

signals:
    void finished();
};