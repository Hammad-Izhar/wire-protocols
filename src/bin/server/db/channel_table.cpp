#include "server/db/channel_table.hpp"

ChannelTable::ChannelTable() {}

Channel ChannelTable::get_by_uid(UUID channel_uid) const
{
    return this->data.at(channel_uid);
}

void ChannelTable::add_channel(Channel &channel)
{
    this->data.insert({channel.get_uid(), channel});
}

void ChannelTable::remove_channel(UUID channel_uid)
{
    this->data.erase(channel_uid);
}