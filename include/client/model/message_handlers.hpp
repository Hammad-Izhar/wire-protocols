#include <QTcpSocket>

#include "message/register_account_response.hpp"

void on_register_account_response(QTcpSocket *socket, RegisterAccountResponse &msg);