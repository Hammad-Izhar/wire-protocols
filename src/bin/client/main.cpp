#include <QApplication>
#include <QPushButton>

#include "client/gui/chat_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ChatWindow window;
    window.show();

    return app.exec();
}