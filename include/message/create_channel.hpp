#pragma once
#include <stdint.h>
#include <string>
#include <vector>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

class CreateChannelMessage : public Serializable {
   public:
    CreateChannelMessage() = default;

    CreateChannelMessage(std::string channel_name, std::vector<UUID> members);

    void serialize(std::vector<uint8_t>& buf) const override;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] size_t size() const override;

    [[nodiscard]] const std::string& get_channel_name() const;

    [[nodiscard]] const std::vector<UUID>& get_members() const;

    void set_channel_name(std::string channel_name);

    void set_members(std::vector<UUID> members);

   private:
    std::string channel_name;
    std::vector<UUID> members;
};