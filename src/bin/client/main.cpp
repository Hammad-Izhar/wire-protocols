#include <QApplication>
#include <QPushButton>

#include "client/model/session.hpp"
#include "client/gui/connection_window.hpp"
#include "client/gui/authentication_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ConnectionWindow window;
    window.show();

    return app.exec();
}
