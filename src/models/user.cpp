#include <algorithm>

#include "models/user.hpp"

User::User(std::string username, std::string display_name)
    : username(username),
      display_name(display_name),
      uid(UUID()),
      profile_pic("./assets/profile_pics/blank_profile_pic.png") {
    // Generate a public/private key pair
    // TBD to friday
}

User::User(std::string username, std::string display_name, UUID uid, std::string profile_pic)
    : username(username), display_name(display_name), uid(uid), profile_pic(profile_pic) {
    // Generate a public/private key pair
    // TBD to friday
}

void User::serialize(std::vector<uint8_t>& buf) const {
    uint8_t username_length = this->username.size();
    uint8_t display_name_length = this->display_name.size();
    uint8_t profile_pic_length = this->profile_pic.size();

    buf.push_back(username_length);
    for (const auto& c : this->username) {
        buf.push_back(c);
    }

    buf.push_back(display_name_length);
    for (const auto& c : this->display_name) {
        buf.push_back(c);
    }

    buf.push_back(profile_pic_length);
    for (const auto& c : this->profile_pic) {
        buf.push_back(c);
    }

    this->uid.serialize(buf);
}

void User::deserialize(const std::vector<uint8_t>& buf) {
    size_t offset = 0;
    uint8_t username_length = buf[offset++];
    uint8_t display_name_length = buf[offset++];
    uint8_t profile_pic_length = buf[offset++];

    this->username = std::string(buf.begin() + offset, buf.begin() + offset + username_length);
    offset += username_length;

    this->display_name =
        std::string(buf.begin() + offset, buf.begin() + offset + display_name_length);
    offset += display_name_length;

    this->profile_pic =
        std::string(buf.begin() + offset, buf.begin() + offset + profile_pic_length);
    offset += profile_pic_length;

    this->uid.deserialize(buf);
}

size_t User::size() const {
    size_t size = 1;  // for the username length
    size += this->username.size();
    size += 1;  // for the display name length
    size += this->display_name.size();
    size += 1;  // for the profile pic length
    size += this->profile_pic.size();
    size += 16;  // for the uid
    return size;
}

const std::string& User::get_username() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->username;
}

const UUID& User::get_uid() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->uid;
}

const std::string& User::get_display_name() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->display_name;
}

const std::string& User::get_profile_pic() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->profile_pic;
}

const std::vector<UUID>& User::get_channels() {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->channels;
}

void User::set_username(std::string username) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->username = username;
}

void User::set_display_name(std::string display_name) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->display_name = display_name;
}

void User::set_profile_pic(std::string profile_pic) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->profile_pic = profile_pic;
}

void User::add_channel(UUID channel_id) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->channels.push_back(channel_id);
}

void User::remove_channel(UUID channel_id) {
    std::lock_guard<std::mutex> lock(this->mutex);
    std::remove_if(this->channels.begin(), this->channels.end(),
                   [channel_id](UUID& c) { return c == channel_id; });
}
