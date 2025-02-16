#include "message/delete_account.hpp"
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

DeleteAccountMessage::DeleteAccountMessage(std::string username, std::string password)
    : username(username), password(password) {}

void DeleteAccountMessage::serialize(std::vector<uint8_t>& buf) const {
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

void DeleteAccountMessage::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::DELETE_ACCOUNT, this->size());
    header.serialize(buf);
    this->serialize(buf);
}

void DeleteAccountMessage::deserialize(const std::vector<uint8_t>& buf) {
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

std::string DeleteAccountMessage::to_json() const {
    nlohmann::json j;
    j["username"] = this->username;
    j["password"] = this->password;
    return j.dump();
}

void DeleteAccountMessage::from_json(const std::string& json) {
    auto j = nlohmann::json::parse(json);
    this->username = j["username"].get<std::string>();
    this->password = j["password"].get<std::string>();
}

size_t DeleteAccountMessage::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    return 2 + this->username.size() + this->password.size();
#endif
}

const std::string& DeleteAccountMessage::get_username() const {
    return this->username;
}

const std::string& DeleteAccountMessage::get_password() const {
    return this->password;
}

void DeleteAccountMessage::set_username(const std::string& username) {
    this->username = username;
}

void DeleteAccountMessage::set_password(const std::string& password) {
    this->password = password;
}