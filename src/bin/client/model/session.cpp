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

void Session::add_message(const Message::SharedPtr& message) {
    if (channel_messages.find(message->get_channel_id()) == channel_messages.end()) {
        return;
    }

    channel_messages[message->get_channel_id()].push_back(message);
    channels[message->get_channel_id()]->add_message(message->get_snowflake());

    if (open_channel && open_channel.value()->get_uid() == message->get_channel_id()) {
        emit updateActiveChannel();
    }
}

const std::vector<Message::SharedPtr>& Session::get_active_channel_messages() const {
    if (open_channel) {
        return channel_messages.at(open_channel.value()->get_uid());
    } else {
        static std::vector<Message::SharedPtr> empty;
        return empty;
    }
}

void Session::set_active_channel(const Channel::SharedPtr& channel) {
    open_channel = channel;
    emit updateActiveChannel();
}