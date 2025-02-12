#include <QTcpSocket>

#include "message/list_accounts.hpp"
#include "message/register_account.hpp"
#include "message/login.hpp"
#include "message/delete_account.hpp"
#include "message/send_message.hpp"

void on_register_account(QTcpSocket *socket, RegisterAccountMessage &msg);

void login(LoginMessage &msg);

void delete_account(DeleteAccountMessage &msg);

void list_accounts(ListAccountsMessage &msg);

void send_message(SendMessageMessage &msg);
