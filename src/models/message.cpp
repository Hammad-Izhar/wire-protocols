#include <cstdint>
#include <mutex>

#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"
#include "models/message.hpp"
#include "models/snowflake.hpp"

Message::Message(UUID sender_id, UUID channel_id, std::string text)
    : sender_id(sender_id), channel_id(channel_id), text(text) {
    SnowflakeIDGenerator& generator = SnowflakeIDGenerator::get_instance();

    this->snowflake = generator.nextId();
    this->created_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
    this->modified_at = this->created_at;
    this->read_by = {this->sender_id};
}

void Message::serialize(std::vector<uint8_t>& buf) const {
    sender_id.serialize(buf);
    channel_id.serialize(buf);
    buf.insert(buf.end(), reinterpret_cast<const uint8_t*>(&snowflake),
               reinterpret_cast<const uint8_t*>(&snowflake) + sizeof(snowflake));
    buf.insert(buf.end(), reinterpret_cast<const uint8_t*>(&created_at),
               reinterpret_cast<const uint8_t*>(&created_at) + sizeof(created_at));
    buf.insert(buf.end(), reinterpret_cast<const uint8_t*>(&modified_at),
               reinterpret_cast<const uint8_t*>(&modified_at) + sizeof(modified_at));
    uint8_t text_length = text.size();
    buf.push_back(text_length);
    buf.insert(buf.end(), text.begin(), text.end());
    uint8_t read_by_size = read_by.size();
    buf.push_back(read_by_size);
    for (const auto& user_id : read_by) {
        user_id.serialize(buf);
    }
}

void Message::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::SEND_MESSAGE, this->size());
    header.serialize(buf);
    serialize(buf);
}

void Message::deserialize(const std::vector<uint8_t>& buf) {
    size_t offset = 0;
    sender_id.deserialize(std::vector<uint8_t>(buf.begin() + offset, buf.begin() + offset + 16));
    offset += 16;
    channel_id.deserialize(std::vector<uint8_t>(buf.begin() + offset, buf.begin() + offset + 16));
    offset += 16;
    snowflake = *reinterpret_cast<const uint64_t*>(&buf[offset]);
    offset += sizeof(snowflake);
    created_at = *reinterpret_cast<const uint64_t*>(&buf[offset]);
    offset += sizeof(created_at);
    modified_at = *reinterpret_cast<const uint64_t*>(&buf[offset]);
    offset += sizeof(modified_at);
    uint8_t text_length = buf[offset++];
    text = std::string(buf.begin() + offset, buf.begin() + offset + text_length);
    offset += text_length;
    uint8_t read_by_size = buf[offset++];
    read_by.clear();
    for (uint32_t i = 0; i < read_by_size; ++i) {
        UUID user_id;
        user_id.deserialize(std::vector<uint8_t>(buf.begin() + offset, buf.begin() + offset + 16));
        read_by.push_back(user_id);
        offset += 16;
    }
}

std::string Message::to_json() const {
    nlohmann::json j;
    j["sender_id"] = sender_id.to_string();
    j["channel_id"] = channel_id.to_string();
    j["snowflake"] = snowflake;
    j["created_at"] = created_at;
    j["modified_at"] = modified_at;
    j["text"] = text;
    std::vector<std::string> read_by_strings;
    for (const auto& user_id : read_by) {
        read_by_strings.push_back(user_id.to_string());
    }
    j["read_by"] = read_by_strings;
    return j.dump();
}

void Message::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    sender_id = UUID::from_string(j["sender_id"].get<std::string>());
    channel_id = UUID::from_string(j["channel_id"].get<std::string>());
    snowflake = j["snowflake"].get<uint64_t>();
    created_at = j["created_at"].get<uint64_t>();
    modified_at = j["modified_at"].get<uint64_t>();
    text = j["text"].get<std::string>();
    read_by.clear();
    for (const auto& user_id : j["read_by"]) {
        read_by.push_back(UUID::from_string(user_id.get<std::string>()));
    }
}

[[nodiscard]] size_t Message::size() const {
    size_t size =
        sender_id.size() + channel_id.size();  // sender_id (16 bytes) + channel_id (16 bytes)
    size += sizeof(snowflake) + sizeof(created_at) + sizeof(modified_at);
    size += 1 + text.size();   // 1 for the text length + text length
    size += sizeof(uint32_t);  // read_by_size
    for (const auto& user_id : read_by) {
        size += 16;  // user_id (16 bytes)
    }
    return size;
}

const uint64_t Message::get_snowflake() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->snowflake;
}

const UUID& Message::get_sender_id() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->sender_id;
}

const UUID& Message::get_channel_id() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->channel_id;
}

const uint64_t Message::get_created_at() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->created_at;
}

const uint64_t Message::get_modified_at() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->modified_at;
}

const std::vector<UUID>& Message::get_read_by() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->read_by;
}

const std::string& Message::get_text() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->text;
}

void Message::set_text(std::string& text) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->text = text;
    this->modified_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();
}

void Message::set_read_by(UUID& user_id) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->read_by.push_back(user_id);
}
