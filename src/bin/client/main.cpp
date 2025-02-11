#include <QApplication>
#include <QPushButton>
#include <thread>

#include "client/gui/connection_window.hpp"
#include "client/gui/authentication_window.hpp"

int gui_main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    AuthenticationWindow window;
    window.show();

    return app.exec();
}

int worker_main()
{

    return 0;
}

int main(int argc, char *argv[])
{
    std::thread gui_thread(gui_main, argc, argv);
    std::thread worker_thread(worker_main);

    gui_thread.join();
    worker_thread.join();

    return 0;
}