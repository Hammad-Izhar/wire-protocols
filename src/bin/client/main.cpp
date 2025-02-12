#include <QApplication>
#include <QStackedLayout>

#include "client/model/session.hpp"
#include "client/gui/connection_window.hpp"
#include "client/gui/authentication_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Session &session = Session::getInstance();
    ConnectionWindow *connectionWindow = new ConnectionWindow();
    AuthenticationWindow *authenticationWindow = new AuthenticationWindow();

    session.main_window->addWidget(connectionWindow);
    session.main_window->addWidget(authenticationWindow);
    session.main_window->setCurrentIndex(0);
    session.main_window->show();

    return app.exec();
}
