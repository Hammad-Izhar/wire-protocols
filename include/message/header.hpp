#pragma once
#include <stdint.h>

#include "message/serialize.hpp"

enum Operation : uint8_t
{
    REGISTER_ACCOUNT = 0,
    LOGIN = 1,
    LIST_ACCOUNT = 2,
    DELETE_ACCOUNT = 3,
    SEND_MESSAGE = 4,
    RECEIVE_MESSAGE = 5,
    DELETE_MESSAGE = 6,
    EDIT_MESSAGE = 7,
    UNREAD_MESSAGE = 8,
    UPDATE_CHANNEL_NAME = 9,
    UPDATE_DISPLAY_NAME = 10,
    UPDATE_PROFILE_PICTURE = 11,
    RESET_PASSWORD = 12,
};

class Header : public Serializable
{
public:
    Header() = default;

    Header(uint8_t version, enum Operation operation, uint16_t packet_length);

    void serialize(std::vector<uint8_t> &buf) const override;

    void deserialize(const std::vector<const uint8_t> &buf) override;

    [[nodiscard]] static size_t size();

    [[nodiscard]] uint8_t get_version() const;

    [[nodiscard]] enum Operation get_operation() const;

    [[nodiscard]] uint16_t get_packet_length() const;

    void set_version(uint8_t version);

    void set_operation(enum Operation operation);

    void set_packet_length(uint16_t packet_length);

private:
    uint8_t version;
    enum Operation operation;
    uint16_t packet_length;
};