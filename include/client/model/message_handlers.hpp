#include <QTcpSocket>

#include "message/login_response.hpp"
#include "message/register_account_response.hpp"

void on_register_account_response(QTcpSocket* socket, RegisterAccountResponse& msg);

void on_login_response(QTcpSocket* socket, LoginResponse& msg);