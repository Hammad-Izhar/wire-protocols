#include <algorithm>

#include "models/channel.hpp"

Channel::Channel(std::string name, std::vector<UUID> user_uids)
    : name(std::move(name)), user_uids(std::move(user_uids)) {
    this->uid = UUID();
}

void Channel::serialize(std::vector<uint8_t>& buf) const {
    this->uid.serialize(buf);

    uint8_t name_length = this->name.size();
    buf.push_back(name_length);
    buf.insert(buf.end(), this->name.begin(), this->name.end());

    uint8_t num_users = this->user_uids.size();
    buf.push_back(num_users);
    for (const UUID& user_uid : this->user_uids) {
        user_uid.serialize(buf);
    }

    uint8_t num_messages = this->message_snowflakes.size();
    buf.push_back(num_messages);
    for (const uint64_t& message_snowflake : this->message_snowflakes) {
        buf.push_back(message_snowflake);
    }
}

void Channel::deserialize(const std::vector<uint8_t>& buf) {
    size_t offset = 0;
    this->uid.deserialize(
        std::vector<uint8_t>(buf.begin() + offset, buf.begin() + offset + this->uid.size()));
    offset += this->uid.size();

    uint8_t name_length = buf[offset++];
    this->name = std::string(buf.begin() + offset, buf.begin() + offset + name_length);
    offset += name_length;

    uint8_t num_users = buf[offset++];
    for (uint8_t i = 0; i < num_users; i++) {
        UUID user_uid;
        user_uid.deserialize(
            std::vector<uint8_t>(buf.begin() + offset, buf.begin() + offset + user_uid.size()));
        this->user_uids.push_back(user_uid);
        offset += user_uid.size();
    }

    uint8_t num_messages = buf[offset++];
    for (uint8_t i = 0; i < num_messages; i++) {
        uint64_t message_snowflake = buf[offset++];
        this->message_snowflakes.push_back(message_snowflake);
    }
}

size_t Channel::size() const {
    size_t size = this->uid.size() + 1 + this->name.size() + 1;  // 1 for name length
    for (const UUID& user_uid : this->user_uids) {
        size += user_uid.size();
    }
    size += 1;                                // for number of messages
    size += this->message_snowflakes.size();  // for each message snowflake
    return size;
}

const UUID& Channel::get_uid() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->uid;
}

const std::string& Channel::get_name() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->name;
}

const std::vector<UUID>& Channel::get_user_uids() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->user_uids;
}

const std::vector<uint64_t>& Channel::get_message_snowflakes() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->message_snowflakes;
}

void Channel::set_name(std::string name) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->name = name;
}

void Channel::add_user(UUID user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->user_uids.push_back(user_uid);
}

void Channel::add_message(const uint64_t& message_snowflake) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->message_snowflakes.push_back(message_snowflake);
}

void Channel::remove_user(const UUID& user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->user_uids.erase(std::remove_if(this->user_uids.begin(), this->user_uids.end(),
                                         [&user_uid](const UUID& uid) { return uid == user_uid; }),
                          this->user_uids.end());
}

void Channel::remove_message(const uint64_t& message_snowflake) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->message_snowflakes.erase(
        std::remove_if(this->message_snowflakes.begin(), this->message_snowflakes.end(),
                       [&message_snowflake](const uint64_t& snowflake) {
                           return snowflake == message_snowflake;
                       }),
        this->message_snowflakes.end());
}