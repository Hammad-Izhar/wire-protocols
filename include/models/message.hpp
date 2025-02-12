#pragma once
#include <stdint.h>
#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <chrono>
#include <memory>

#include "models/uuid.hpp"

class Message
{
public:
    typedef std::shared_ptr<Message> SharedPtr;

    Message(UUID sender_id, UUID channel_id, std::string text);

    // Getters
    [[nodiscard]] const UUID &get_sender_id() const;
    [[nodiscard]] const UUID &get_channel_id() const;
    [[nodiscard]] uint64_t get_created_at() const;
    [[nodiscard]] uint64_t get_modified_at() const;
    [[nodiscard]] const std::vector<UUID> &get_read_by() const;
    [[nodiscard]] const std::string &get_text() const;
    [[nodiscard]] uint64_t get_snowflake() const;

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
};