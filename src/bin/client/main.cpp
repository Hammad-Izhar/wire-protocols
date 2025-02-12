#include <QApplication>
#include <QStackedLayout>

#include "client/model/session.hpp"
#include "client/gui/connection_window.hpp"
#include "client/gui/authentication_window.hpp"
#include "client/gui/chat_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Session &session = Session::getInstance();
    ConnectionWindow *connectionWindow = new ConnectionWindow();
    AuthenticationWindow *authenticationWindow = new AuthenticationWindow();
    ChatWindow *chatWindow = new ChatWindow();

    session.main_window->addWidget(connectionWindow);
    session.main_window->addWidget(authenticationWindow);
    session.main_window->addWidget(chatWindow);
    session.main_window->setCurrentIndex(2);
    session.main_window->show();

    return app.exec();
}
