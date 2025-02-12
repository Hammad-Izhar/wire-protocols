#include <QCoreApplication>
#include <iostream>

#include "message/delete_account.hpp"
#include "message/delete_message.hpp"
#include "message/list_accounts.hpp"
#include "message/login.hpp"
#include "message/register_account.hpp"
#include "message/send_message.hpp"
#include "models/message_handler.hpp"
#include "server/model/message_handlers.hpp"
#include "server/model/tcp_server.hpp"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    TcpServer server;

    MessageHandler& handler = MessageHandler::get_instance();

    handler.register_handler<RegisterAccountMessage>(&on_register_account);
    handler.register_handler<LoginMessage>(&on_login);

    if (!server.listen(QHostAddress::Any, 12345)) {
        std::cerr << "TCP Server failed to start: " << server.errorString().toStdString()
                  << std::endl;
        return -1;
    }

    std::cout << "Server started on port 12345" << std::endl;
    return app.exec();
}