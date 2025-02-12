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
