#include "message/login_response.hpp"
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

LoginResponse::LoginResponse(std::variant<User::SharedPtr, std::string> data)
    : data(std::move(data)) {}

void LoginResponse::serialize(std::vector<uint8_t>& buf) const {
#if PROTOCOL_JSON
    std::string msg = to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    if (std::holds_alternative<User::SharedPtr>(data)) {
        buf.push_back(0);
        std::get<User::SharedPtr>(data)->serialize(buf);
    } else {
        buf.push_back(1);
        const std::string& error = std::get<std::string>(data);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
#endif
}

void LoginResponse::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::LOGIN, this->size());
    header.serialize(buf);
    serialize(buf);
}

void LoginResponse::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    std::string msg(buf.begin(), buf.end());
    from_json(msg);
#else
    size_t offset = 0;
    uint8_t has_error = buf[offset++];
    if (has_error == 0) {
        User::SharedPtr user = std::make_shared<User>();
        user->deserialize(std::vector<uint8_t>(buf.begin() + offset, buf.end()));
        data = user;
    } else {
        uint8_t error_length = buf[offset++];
        std::string error(buf.begin() + offset, buf.begin() + offset + error_length);
        data = error;
    }
#endif
}

std::string LoginResponse::to_json() const {
    nlohmann::json j;
    if (std::holds_alternative<User::SharedPtr>(data)) {
        j["user"] = std::get<User::SharedPtr>(data)->to_json();
    } else {
        j["error"] = std::get<std::string>(data);
    }
    return j.dump();
}

void LoginResponse::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    if (j.contains("user")) {
        User::SharedPtr user = std::make_shared<User>();
        user->from_json(j["user"]);
        data = user;
    } else {
        data = j["error"].get<std::string>();
    }
}

[[nodiscard]] size_t LoginResponse::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    size_t size = 1;  // for the has_error byte
    if (std::holds_alternative<User::SharedPtr>(data)) {
        size += std::get<User::SharedPtr>(data)->size();
    } else {
        const std::string& error = std::get<std::string>(data);
        size += 1 + error.size();  // 1 for the error length + error length
    }
    return size;
#endif
}

[[nodiscard]] bool LoginResponse::is_success() const {
    return !std::holds_alternative<std::string>(data);
}

const std::optional<User::SharedPtr> LoginResponse::get_data() const {
    if (is_success()) {
        return std::get<User::SharedPtr>(data);
    } else {
        return std::nullopt;
    }
}

[[nodiscard]] const std::optional<std::string> LoginResponse::get_error_message() const {
    if (is_success()) {
        return std::nullopt;
    } else {
        return std::get<std::string>(data);
    }
}