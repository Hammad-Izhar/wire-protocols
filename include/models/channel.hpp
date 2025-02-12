#pragma once
#include <stdint.h>
#include "models/user.hpp"
#include "models/message.hpp"
#include "models/UUID.hpp"
#include <array>
#include <string>

class Channel 
{
public:
    Channel(std::string name, std::vector<std::array<uint8_t,16> > user_uids);

    // Getters
    [[nodiscard]] const UUID &get_uid() const;
    [[nodiscard]] const std::string &get_name() const;
    [[nodiscard]] const std::vector<UUID> &get_user_uids() const;
    [[nodiscard]] const std::vector<uint64_t> &get_message_snowflakes() const;


private:

    UUID uid;
    std::string name;
    std::vector<UUID> user_uids;
    std::vector<uint64_t> message_snowflakes;
};