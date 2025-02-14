#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"

enum Operation : uint8_t {
    REGISTER_ACCOUNT,
    LOGIN,
    LIST_ACCOUNTS,
    DELETE_ACCOUNT,
    SEND_MESSAGE,
    READ_MESSAGE,
    DELETE_MESSAGE,
    EDIT_MESSAGE,
    UNREAD_MESSAGE,
    CREATE_CHANNEL,
    UPDATE_CHANNEL_NAME,
    UPDATE_DISPLAY_NAME,
    UPDATE_PROFILE_PICTURE,
    RESET_PASSWORD,
};

class Header : public Serializable {
   public:
    Header() = default;

    Header(uint8_t version, enum Operation operation, uint16_t packet_length);

    void serialize(std::vector<uint8_t>& buf) const override;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] size_t size() const override;

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