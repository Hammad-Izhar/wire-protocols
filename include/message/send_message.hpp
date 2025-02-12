#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

class SendMessageMessage : public Serializable
{
public:
    SendMessageMessage() = default;

    SendMessageMessage(UUID channel_uid, UUID sender_uid, std::string text);

    void serialize(std::vector<uint8_t> &buf) const override;

    void serialize_msg(std::vector<uint8_t> &buf) const;

    void deserialize(const std::vector<uint8_t> &buf) override;

    [[nodiscard]] std::string get_text() const;
    [[nodiscard]] UUID get_sender_uid() const;
    [[nodiscard]] UUID get_channel_uid() const;


    [[nodiscard]] size_t size() const;

    void set_regex(std::string regex);

private:
    UUID channel_uid;
    UUID sender_uid;
    std::string text;
};