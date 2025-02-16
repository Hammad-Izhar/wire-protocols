#include <algorithm>
#include <cstdint>

#include "json.hpp"
#include "models/user.hpp"

User::User(std::string username, std::string display_name)
    : username(username),
      display_name(display_name),
      uid(UUID()),
      profile_pic(":/assets/profile_pics/blank_profile_pic.png") {}

User::User(std::string username, std::string display_name, UUID uid, std::string profile_pic)
    : username(username), display_name(display_name), uid(uid), profile_pic(profile_pic) {}

void User::serialize(std::vector<uint8_t>& buf) const {
#if PROTOCOL_JSON
    std::string msg = to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    uint8_t username_length = this->username.size();
    uint8_t display_name_length = this->display_name.size();
    uint8_t profile_pic_length = this->profile_pic.size();

    this->uid.serialize(buf);

    buf.push_back(username_length);
    buf.insert(buf.end(), this->username.begin(), this->username.end());

    buf.push_back(display_name_length);
    buf.insert(buf.end(), this->display_name.begin(), this->display_name.end());

    buf.push_back(profile_pic_length);
    buf.insert(buf.end(), this->profile_pic.begin(), this->profile_pic.end());
#endif
}

void User::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    std::string msg(buf.begin(), buf.end());
    from_json(msg);
#else
    size_t offset = 0;
    this->uid.deserialize(buf);
    offset += this->uid.size();

    uint8_t username_length = buf[offset++];
    this->username = std::string(buf.begin() + offset, buf.begin() + offset + username_length);
    offset += username_length;

    uint8_t display_name_length = buf[offset++];
    this->display_name =
        std::string(buf.begin() + offset, buf.begin() + offset + display_name_length);
    offset += display_name_length;

    uint8_t profile_pic_length = buf[offset++];
    this->profile_pic =
        std::string(buf.begin() + offset, buf.begin() + offset + profile_pic_length);
    offset += profile_pic_length;
#endif
}

std::string User::to_json() const {
    nlohmann::json j;
    j["uid"] = this->uid.to_string();
    j["username"] = this->username;
    j["display_name"] = this->display_name;
    j["profile_pic"] = this->profile_pic;
    return j.dump();
}

void User::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    this->uid = UUID::from_string(j["uid"].get<std::string>());
    this->username = j["username"].get<std::string>();
    this->display_name = j["display_name"].get<std::string>();
    this->profile_pic = j["profile_pic"].get<std::string>();
}

size_t User::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    size_t size = this->uid.size();
    size += 1 + this->username.size();      // for the username and its length byte
    size += 1 + this->display_name.size();  // for the display name and its length byte
    size += 1 + this->profile_pic.size();   // for the profile pic and its length byte

    return size;
#endif
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
    auto it = std::find(this->channels.begin(), this->channels.end(), channel_id);
    if (it != this->channels.end()) {
        this->channels.erase(it);
    }
}
