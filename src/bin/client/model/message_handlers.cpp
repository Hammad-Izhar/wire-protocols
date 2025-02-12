#include "client/model/message_handlers.hpp"
#include "client/model/session.hpp"

void on_register_account_response(QTcpSocket *socket, RegisterAccountResponse &msg)
{
    Session &session = Session::getInstance();
    if (msg.is_success())
    {
        emit session.tcp_client->registrationSuccess();
    }
    else
    {
        emit session.tcp_client->registrationFailure(QString::fromStdString(msg.get_error_message().value()));
    }
};