#pragma once
#include <stdint.h>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <variant>

#include "models/channel.hpp"
#include "models/uuid.hpp"

class ChannelTable {
   public:
    ChannelTable() = default;

    [[nodiscard]] std::optional<const Channel::SharedPtr> get_by_uid(UUID channel_uid);

    [[nodiscard]] std::optional<Channel::SharedPtr> get_mut_by_uid(UUID channel_uid);

    // Setters
    std::variant<Channel::SharedPtr, std::string> add_channel(std::string channel_name,
                                                              std::vector<UUID> members);
    std::variant<std::monostate, std::string> remove_channel(UUID channel_uid);

   private:
    std::unordered_map<UUID, Channel::SharedPtr> data;
    std::mutex mutex;
};