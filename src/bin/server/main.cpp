#include <QCoreApplication>
#include <iostream>

#include "server/model/tcp_server.hpp"
#include "models/message_handler.hpp"
#include "message/register_account.hpp"
#include "message/login.hpp"
#include "message/delete_account.hpp"
#include "message/list_accounts.hpp"
#include "message/send_message.hpp"
#include "message/delete_message.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    TcpServer server;

    MessageHandler &handler = MessageHandler::get_instance();

    handler.register_handler<RegisterAccountMessage>([](RegisterAccountMessage &msg)
                                                     { std::cout << "RegisterAccount message received" << std::endl; });
    handler.register_handler<LoginMessage>([](LoginMessage &msg)
                                           { std::cout << "Login message received" << std::endl; });
    handler.register_handler<DeleteAccountMessage>([](DeleteAccountMessage &msg)
                                                   { std::cout << "DeleteAccount message received" << std::endl; });
    handler.register_handler<ListAccountsMessage>([](ListAccountsMessage &msg)
                                                  { std::cout << "ListAccounts message received" << std::endl; });
    handler.register_handler<SendMessageMessage>([](SendMessageMessage &msg)
                                                 { std::cout << "SendMessage message received" << std::endl; });
    handler.register_handler<DeleteMessageMessage>([](DeleteMessageMessage &msg)
                                                 { std::cout << "DeleteMessage message received" << std::endl; });

    if (!server.listen(QHostAddress::Any, 12345))
    {
        std::cerr << "TCP Server failed to start: "
                  << server.errorString().toStdString() << std::endl;
        return -1;
    }

    std::cout << "Server started on port 12345" << std::endl;
    return app.exec();
}