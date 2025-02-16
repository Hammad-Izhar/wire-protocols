#include "message/register_account_response.hpp"
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

RegisterAccountResponse::RegisterAccountResponse(
    std::variant<std::monostate, std::string> error_message)
    : error_message(std::move(error_message)) {}

void RegisterAccountResponse::serialize(std::vector<uint8_t>& buf) const {
#if PROTOCOL_JSON
    std::string msg = to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    if (std::holds_alternative<std::monostate>(error_message)) {
        buf.push_back(0);
    } else {
        buf.push_back(1);
        const std::string& error = std::get<std::string>(error_message);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
#endif
}

void RegisterAccountResponse::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::REGISTER_ACCOUNT, this->size());
    header.serialize(buf);
    serialize(buf);
}

void RegisterAccountResponse::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    std::string msg(buf.begin(), buf.end());
    from_json(msg);
#else
    size_t offset = 0;
    uint8_t has_error = buf[offset++];
    if (has_error == 0) {
        error_message = std::monostate();
    } else {
        uint8_t error_length = buf[offset++];
        std::string error(buf.begin() + offset, buf.begin() + offset + error_length);
        error_message = error;
    }
#endif
}

std::string RegisterAccountResponse::to_json() const {
    nlohmann::json j;
    if (std::holds_alternative<std::monostate>(error_message)) {
        j["success"] = true;
    } else {
        j["success"] = false;
        j["error"] = std::get<std::string>(error_message);
    }
    return j.dump();
}

void RegisterAccountResponse::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    if (j["success"].get<bool>()) {
        error_message = std::monostate();
    } else {
        error_message = j["error"].get<std::string>();
    }
}

[[nodiscard]] size_t RegisterAccountResponse::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    size_t size = 1;  // for the has_error byte
    if (std::holds_alternative<std::string>(error_message)) {
        const std::string& error = std::get<std::string>(error_message);
        size += 1 + error.size();  // 1 for the error length + error length
    }
    return size;
#endif
}

[[nodiscard]] bool RegisterAccountResponse::is_success() const {
    return std::holds_alternative<std::monostate>(error_message);
}

[[nodiscard]] const std::optional<std::string> RegisterAccountResponse::get_error_message() const {
    if (is_success()) {
        return std::nullopt;
    } else {
        return std::get<std::string>(error_message);
    }
}