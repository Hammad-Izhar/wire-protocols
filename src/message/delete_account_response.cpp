#include "message/delete_account_response.hpp"
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

DeleteAccountResponse::DeleteAccountResponse(std::variant<User::SharedPtr, std::string> data)
    : data(data) {}

void DeleteAccountResponse::serialize(std::vector<uint8_t>& buf) const {
#if PROTOCOL_JSON
    std::string msg = to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    if (is_success()) {
        buf.push_back(0);
        std::get<User::SharedPtr>(data)->serialize(buf);
    } else {
        buf.push_back(1);
        std::string error = std::get<std::string>(data);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
#endif
}

void DeleteAccountResponse::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::DELETE_ACCOUNT, size());
    header.serialize(buf);
    serialize(buf);
}

void DeleteAccountResponse::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    std::string msg(buf.begin(), buf.end());
    from_json(msg);
#else
    if (buf[0] == 0) {
        User::SharedPtr user = std::make_shared<User>();
        user->deserialize(std::vector<uint8_t>(buf.begin() + 1, buf.end()));
        data = user;
    } else {
        size_t len = buf[1];
        std::string error(buf.begin() + 2, buf.begin() + 2 + len);
        data = error;
    }
#endif
}

std::string DeleteAccountResponse::to_json() const {
    nlohmann::json j;
    if (is_success()) {
        j["success"] = true;
        j["user"] = std::get<User::SharedPtr>(data)->to_json();
    } else {
        j["success"] = false;
        j["error"] = std::get<std::string>(data);
    }
    return j.dump();
}

void DeleteAccountResponse::from_json(const std::string& json) {
    auto j = nlohmann::json::parse(json);
    if (j["success"].get<bool>()) {
        User::SharedPtr user = std::make_shared<User>();
        user->from_json(j["user"]);
        data = user;
    } else {
        data = j["error"].get<std::string>();
    }
}

size_t DeleteAccountResponse::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    size_t size = 1;
    if (is_success()) {
        size += std::get<User::SharedPtr>(data)->size();
    } else {
        size += 1 + std::get<std::string>(data).size();
    }
    return size;
#endif
}

bool DeleteAccountResponse::is_success() const {
    return std::holds_alternative<User::SharedPtr>(data);
}

std::optional<std::string> DeleteAccountResponse::get_error_message() const {
    if (std::holds_alternative<std::string>(data)) {
        return std::get<std::string>(data);
    }
    return std::nullopt;
}

std::optional<User::SharedPtr> DeleteAccountResponse::get_data() const {
    if (std::holds_alternative<User::SharedPtr>(data)) {
        return std::get<User::SharedPtr>(data);
    }
    return std::nullopt;
}