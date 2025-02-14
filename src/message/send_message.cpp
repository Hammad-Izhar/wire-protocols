#include "message/send_message.hpp"
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"
#include "models/uuid.hpp"

SendMessageMessage::SendMessageMessage(UUID channel_uid, UUID sender_uid, std::string text)
    : channel_uid(channel_uid), sender_uid(sender_uid), text(text) {}

void SendMessageMessage::serialize(std::vector<uint8_t>& buf) const {
    // Encode channel UUID
    this->channel_uid.serialize(buf);

    // Encode sender UUID
    this->sender_uid.serialize(buf);

    // Encode text length and text
    uint8_t text_length = this->text.size();
    buf.push_back(text_length);
    buf.insert(buf.end(), this->text.begin(), this->text.end());
}

void SendMessageMessage::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::SEND_MESSAGE, this->size());
    header.serialize(buf);
    this->serialize(buf);
}

void SendMessageMessage::deserialize(const std::vector<uint8_t>& buf) {
    size_t offset = 0;
    this->channel_uid.deserialize(std::vector<uint8_t>(
        buf.begin() + offset, buf.begin() + offset + this->channel_uid.size()));
    offset += this->channel_uid.size();

    this->sender_uid.deserialize(
        std::vector<uint8_t>(buf.begin() + offset, buf.begin() + offset + this->sender_uid.size()));
    offset += this->sender_uid.size();

    uint8_t text_length = buf[offset++];
    this->text = std::string(buf.begin() + offset, buf.begin() + offset + text_length);
}

std::string SendMessageMessage::to_json() const {
    nlohmann::json j;
    j["channel_uid"] = this->channel_uid.to_string();
    j["sender_uid"] = this->sender_uid.to_string();
    j["text"] = this->text;
    return j.dump();
}

void SendMessageMessage::from_json(const std::string& json) {
    auto j = nlohmann::json::parse(json);
    this->channel_uid = UUID::from_string(j["channel_uid"].get<std::string>());
    this->sender_uid = UUID::from_string(j["sender_uid"].get<std::string>());
    this->text = j["text"].get<std::string>();
}

size_t SendMessageMessage::size() const {
    return this->channel_uid.size() + this->sender_uid.size() + 1 + this->text.size();
}

UUID SendMessageMessage::get_channel_uid() const {
    return this->channel_uid;
}

UUID SendMessageMessage::get_sender_uid() const {
    return this->sender_uid;
}

std::string SendMessageMessage::get_text() const {
    return this->text;
}
