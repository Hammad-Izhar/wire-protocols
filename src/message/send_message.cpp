#include "message/send_message.hpp"
#include "constants.hpp"
#include "message/header.hpp"

SendMessageMessage::SendMessageMessage(UUID channel_uid, UUID sender_uid, std::string text)
    : channel_uid(channel_uid), sender_uid(sender_uid), text(text) {}

void SendMessageMessage::serialize(std::vector<uint8_t> &buf) const
{
    // Encode channel UUID
    this->channel_uid.serialize(buf);

    // Encode sender UUID
    this->sender_uid.serialize(buf);

    // Encode text length and text
    uint8_t text_length = this->text.size();

    buf.push_back(text_length);
    for (const auto &c : this->text)
    {
        buf.push_back(c);
    }
}

void SendMessageMessage::serialize_msg(std::vector<uint8_t> &buf) const
{
    Header header(PROTOCOL_VERSION, Operation::SEND_MESSAGE, this->size());
    header.serialize(buf);
    this->serialize(buf);
}

void SendMessageMessage::deserialize(const std::vector<uint8_t> &buf)
{
    // Decode channel UUID
    this->channel_uid.deserialize(std::vector<uint8_t>(buf.begin(), buf.begin() + 16));

    // Decode sender UUID
    this->sender_uid.deserialize(std::vector<uint8_t>(buf.begin() + 16, buf.begin() + 32));

    // Decode text length and text
    uint8_t text_length = buf[32];
    this->text = std::string(buf.begin() + 33, buf.begin() + 33 + text_length);
}
size_t SendMessageMessage::size() const
{
    return 17 + this->text.size();
}

UUID SendMessageMessage::get_channel_uid() const
{
    return this->channel_uid;
}

UUID SendMessageMessage::get_sender_uid() const
{
    return this->sender_uid;
}

std::string SendMessageMessage::get_text() const
{
    return this->text;
}
