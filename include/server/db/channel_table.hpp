#pragma once
#include <stdint.h>
#include <unordered_map>
#include <optional>
#include <variant>
#include <mutex>

#include "models/channel.hpp"
#include "models/uuid.hpp"

class ChannelTable
{
public:
    ChannelTable() = default;

    [[nodiscard]] std::optional<const Channel::SharedPtr> get_by_uid(UUID channel_uid);

    [[nodiscard]] std::optional<Channel::SharedPtr> get_mut_by_uid(UUID channel_uid);

    // Setters
    std::variant<std::monostate, std::string> add_channel(Channel::SharedPtr channel);
    std::variant<std::monostate, std::string> remove_channel(UUID channel_uid);

private:
    std::unordered_map<UUID, Channel::SharedPtr> data;
    std::mutex mutex;
};