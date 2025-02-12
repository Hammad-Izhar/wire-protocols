#pragma once
#include <stdint.h>
#include <unordered_map>
#include <optional>
#include <variant>

#include "models/channel.hpp"
#include "models/uuid.hpp"

class ChannelTable
{
public:
    ChannelTable();

    [[nodiscard]] std::optional<const Channel::SharedPtr> get_by_uid(UUID channel_uid) const;

    [[nodiscard]] std::optional<Channel::SharedPtr> get_mut_by_uid(UUID channel_uid);

    // Setters
    std::variant<void, std::string> add_channel(Channel::SharedPtr channel);
    std::variant<void, std::string> remove_channel(UUID channel_uid);

private:
    std::unordered_map<UUID, Channel::SharedPtr> data;
};