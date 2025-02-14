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

std::optional<Channel::SharedPtr> Session::get_active_channel() const {
    return open_channel;
}

void Session::set_active_channel(const Channel::SharedPtr& channel) {
    open_channel = channel;
    emit updateActiveChannel();
}