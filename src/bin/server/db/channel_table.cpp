#include "server/db/channel_table.hpp"

std::optional<const Channel::SharedPtr> ChannelTable::get_by_uid(UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(channel_uid) != this->data.end()
               ? std::optional<const Channel::SharedPtr>(this->data.at(channel_uid))
               : std::nullopt;
}

std::optional<Channel::SharedPtr> ChannelTable::get_mut_by_uid(UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(channel_uid) != this->data.end()
               ? std::optional<Channel::SharedPtr>(this->data.at(channel_uid))
               : std::nullopt;
}

std::variant<Channel::SharedPtr, std::string> ChannelTable::add_channel(std::string channel_name,
                                                                        std::vector<UUID> members) {
    std::lock_guard<std::mutex> lock(this->mutex);
    Channel::SharedPtr channel = std::make_shared<Channel>(channel_name, members);
    this->data.insert({channel->get_uid(), channel});

    return channel;
}

std::variant<std::monostate, std::string> ChannelTable::remove_channel(UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.erase(channel_uid);

    return {};
}