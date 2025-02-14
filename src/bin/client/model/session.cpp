#include <QWidget>
#include <optional>

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

void Session::reset() {
    authenticated_user = std::nullopt;
    channels.clear();
    channel_messages.clear();
    open_channel = std::nullopt;
}

void Session::add_message(const Message::SharedPtr& message) {
    if (channel_messages.find(message->get_channel_id()) == channel_messages.end()) {
        return;
    }

    channel_messages[message->get_channel_id()].push_back(message);
    channels[message->get_channel_id()]->add_message(message->get_snowflake());
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

void Session::add_channel(const Channel::SharedPtr& channel) {
    channels[channel->get_uid()] = channel;
    channel_messages[channel->get_uid()] = {};
}

void Session::remove_message(const Message::SharedPtr& message) {
    if (channel_messages.find(message->get_channel_id()) == channel_messages.end()) {
        return;
    }

    auto& messages = channel_messages[message->get_channel_id()];
    messages.erase(std::remove(messages.begin(), messages.end(), message), messages.end());
    channels[message->get_channel_id()]->remove_message(message->get_snowflake());
}

std::optional<UUID> Session::get_active_user_id() const {
    if (authenticated_user) {
        return authenticated_user.value()->get_uid();
    } else {
        return std::nullopt;
    }
}