#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include <memory>

#include "models/user.hpp"
#include "models/message.hpp"
#include "models/uuid.hpp"

class Channel
{
public:
    typedef std::shared_ptr<Channel> SharedPtr;

    Channel(std::string name, std::vector<UUID> user_uids);

    // Getters
    [[nodiscard]] const UUID &get_uid() const;
    [[nodiscard]] const std::string &get_name() const;
    [[nodiscard]] const std::vector<UUID> &get_user_uids() const;
    [[nodiscard]] const std::vector<uint64_t> &get_message_snowflakes() const;

    // Setters
    void set_name(std::string name);
    void add_user(UUID user_uid);
    void add_message(const uint64_t &message_snowflake);
    void remove_user(const UUID &user_uid);
    void remove_message(const uint64_t &message_snowflake);

private:
    UUID uid;
    std::string name;
    std::vector<UUID> user_uids;
    std::vector<uint64_t> message_snowflakes;
};