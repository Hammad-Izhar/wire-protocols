#include "message/login.hpp"
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

LoginMessage::LoginMessage(std::string username, std::string password)
    : username(username), password(password) {}

void LoginMessage::serialize(std::vector<uint8_t>& buf) const {
#if PROTOCOL_JSON
    std::string msg = to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    uint8_t username_length = this->username.size();
    uint8_t password_length = this->password.size();

    buf.push_back(username_length);
    for (const auto& c : this->username) {
        buf.push_back(c);
    }

    buf.push_back(password_length);
    for (const auto& c : this->password) {
        buf.push_back(c);
    }
#endif
}

void LoginMessage::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::LOGIN, this->size());
    header.serialize(buf);
    this->serialize(buf);
}

void LoginMessage::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    std::string msg(buf.begin(), buf.end());
    from_json(msg);
#else
    uint8_t username_length = buf[0];
    uint8_t password_length = buf[username_length + 1];

    this->username = std::string(buf.begin() + 1, buf.begin() + 1 + username_length);
    this->password = std::string(buf.begin() + username_length + 2,
                                 buf.begin() + username_length + 2 + password_length);
#endif
}

std::string LoginMessage::to_json() const {
    nlohmann::json j;
    j["username"] = this->username;
    j["password"] = this->password;
    return j.dump();
}

void LoginMessage::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    this->username = j["username"];
    this->password = j["password"];
}

size_t LoginMessage::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    return 2 + this->username.size() + this->password.size();
#endif
}

const std::string& LoginMessage::get_username() const {
    return this->username;
}

const std::string& LoginMessage::get_password() const {
    return this->password;
}

void LoginMessage::set_username(const std::string& username) {
    this->username = username;
}

void LoginMessage::set_password(const std::string& password) {
    this->password = password;
}