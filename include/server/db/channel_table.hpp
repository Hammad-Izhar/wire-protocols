#pragma once
#include <stdint.h>
#include <unordered_map>

#include "models/channel.hpp"
#include "models/uuid.hpp"

class ChannelTable
{
public:
    ChannelTable();

    [[nodiscard]] Channel get_by_uid(UUID channel_uid) const;

    // Setters
    void add_channel(Channel &channel);
    void remove_channel(UUID channel_uid);

private:
    std::unordered_map<UUID, Channel> data;
};