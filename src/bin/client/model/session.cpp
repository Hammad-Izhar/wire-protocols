#include <QWidget>

#include "client/model/session.hpp"

Session& Session::get_instance() {
    static Session instance;
    if (!instance.tcp_client)
        instance.tcp_client = new TcpClient();
    if (!instance.main_window)
        instance.main_window = new StackedWindow();
    return instance;
}