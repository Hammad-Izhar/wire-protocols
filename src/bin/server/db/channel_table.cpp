#include "server/db/channel_table.hpp"

ChannelTable::ChannelTable() {}

std::optional<const Channel::SharedPtr> ChannelTable::get_by_uid(UUID channel_uid) const
{
    return this->data.find(channel_uid) != this->data.end() ? std::optional<const Channel::SharedPtr>(this->data.at(channel_uid)) : std::nullopt;
}

std::optional<Channel::SharedPtr> ChannelTable::get_mut_by_uid(UUID channel_uid)
{
    return this->data.find(channel_uid) != this->data.end() ? std::optional<Channel::SharedPtr>(this->data.at(channel_uid)) : std::nullopt;
}

std::variant<void, std::string> ChannelTable::add_channel(Channel::SharedPtr channel)
{
    this->data.insert({channel->get_uid(), channel});
}

std::variant<void, std::string> ChannelTable::remove_channel(UUID channel_uid)
{
    this->data.erase(channel_uid);
}