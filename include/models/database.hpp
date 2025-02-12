#pragma once
#include <unordered_map>
#include <stdint.h>

#include "server/db/user_db.hpp"
#include "server/db/message_db.hpp"
#include "server/db/channel_db.hpp"
#include "models/uuid.hpp"
#include "models/user.hpp"
#include "models/message.hpp"
#include "models/channel.hpp"

class database
{
public:
    database();
    // Getters
    [[nodiscard]] User &get_user_by_uid(UUID user_uid);
    [[nodiscard]] const Message &get_message_by_uid(uint64_t message_snowflake) const;
    [[nodiscard]] const Channel &get_channel_by_uid(UUID channel_uid) const;

    // Setters -- Add
    void add_user(User &user);
    void add_message(Message &message);
    void add_channel(Channel &channel);
    void add_user_to_channel(UUID user_uid, UUID channel_uid);

    // Setters -- Remove
    void remove_user(UUID user_uid);
    void remove_message(uint64_t message_snowflake);
    void remove_channel(UUID channel_uid);

private:
    user_db users;
    message_db messages;
    channel_db channels;
};
