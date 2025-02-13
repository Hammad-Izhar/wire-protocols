#include <QCoreApplication>
#include <iostream>

#include "models/message_handler.hpp"
#include "server/model/message_handlers.hpp"
#include "server/model/tcp_server.hpp"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    TcpServer server;

    init_message_handlers();

    if (!server.listen(QHostAddress::Any, 12345)) {
        std::cerr << "TCP Server failed to start: " << server.errorString().toStdString()
                  << std::endl;
        return -1;
    }

    std::cout << "Server started on port 12345" << std::endl;
    return app.exec();
}