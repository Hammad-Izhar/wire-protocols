#pragma once
#include <stdint.h>
#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <chrono>
#include <memory>
#include <mutex>

#include "models/uuid.hpp"

class Message
{
public:
    typedef std::shared_ptr<Message> SharedPtr;

    Message(UUID sender_id, UUID channel_id, std::string text);

    // Getters
    [[nodiscard]] const uint64_t get_snowflake();
    [[nodiscard]] const UUID &get_sender_id();
    [[nodiscard]] const UUID &get_channel_id();
    [[nodiscard]] const uint64_t get_created_at();
    [[nodiscard]] const uint64_t get_modified_at();
    [[nodiscard]] const std::vector<UUID> &get_read_by();
    [[nodiscard]] const std::string &get_text();

    // Setters
    void set_text(std::string &text);
    void set_read_by(UUID &user_id);

private:
    uint64_t snowflake;
    UUID sender_id;
    UUID channel_id;
    uint64_t created_at;
    uint64_t modified_at;
    std::vector<UUID> read_by;
    std::string text;
    std::mutex mutex;
};