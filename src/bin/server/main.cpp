#include <QCoreApplication>
#include <iostream>

#include "server/model/tcp_server.hpp"
#include "models/message_handler.hpp"
#include "message/register_account.hpp"
#include "message/login.hpp"
#include "message/delete_account.hpp"
#include "message/list_accounts.hpp"
#include "message/send_message.hpp"
#include "server/model/message_handlers.hpp"
#include "message/delete_message.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TcpServer server;

    MessageHandler &handler = MessageHandler::get_instance();

    handler.register_handler<RegisterAccountMessage>(&on_register_account);

    if (!server.listen(QHostAddress::Any, 12345))
    {
        std::cerr << "TCP Server failed to start: "
                  << server.errorString().toStdString() << std::endl;
        return -1;
    }

    std::cout << "Server started on port 12345" << std::endl;
    return app.exec();
}