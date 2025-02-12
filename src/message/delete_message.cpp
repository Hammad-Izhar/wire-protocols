#include "message/delete_message.hpp"
#include "constants.hpp"
#include "message/header.hpp"

DeleteMessageMessage::DeleteMessageMessage(UUID channel_uid, uint64_t message_snowflake)
    : channel_uid(channel_uid), message_snowflake(message_snowflake) {}

void DeleteMessageMessage::serialize(std::vector<uint8_t>& buf) const {
    // Encode channel UUID
    this->channel_uid.serialize(buf);
    // Encode message snowflake
    for (int i = 56; i >= 0; i -= 8) {
        buf.push_back((this->message_snowflake >> i));
    }
}

void DeleteMessageMessage::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::DELETE_MESSAGE, this->size());
    header.serialize(buf);
    this->serialize(buf);
}

void DeleteMessageMessage::deserialize(const std::vector<uint8_t>& buf) {
    // Decode channel UUID
    this->channel_uid.deserialize(std::vector<uint8_t>(buf.begin(), buf.begin() + 16));
    // Decode message snowflake
    this->message_snowflake = 0;
    for (int i = 0; i < 8; i++) {
        this->message_snowflake = (this->message_snowflake << 8) | buf[16 + i];
    }
}

size_t DeleteMessageMessage::size() const {
    return 24;
}

uint64_t DeleteMessageMessage::get_message_snowflake() const {
    return this->message_snowflake;
}