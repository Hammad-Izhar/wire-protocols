#pragma once
#include <stdint.h>
#include <vector>
#include <ctime>
#include <array>
#include <string>
#include <cstdint>

class Message 
{
public:
    Message(std::array<uint8_t,16> sender_id, std::array<uint8_t,16> channel_id, std::string text);

    // Getters
    [[nodiscard]] const std::array<uint8_t,16> &get_sender_id() const;
    [[nodiscard]] const std::array<uint8_t,16> &get_channel_id() const;
    [[nodiscard]] time_t get_created_at() const;
    [[nodiscard]] time_t get_modified_at() const;
    [[nodiscard]] const std::vector<std::array<uint8_t,16>> &get_read_by() const;
    [[nodiscard]] const std::string &get_text() const;
    [[nodiscard]] uint64_t get_snowflake() const;

    // Setters
    void set_text(std::string &text);
    void set_read_by(std::array<uint8_t,16> &user_id);

private:
    uint64_t snowflake;
    std::array<uint8_t,16> sender_id;
    std::array<uint8_t,16> channel_id;
    time_t created_at;
    time_t modified_at;
    std::vector<std::array<uint8_t,16>> read_by;
    std::string text;
};