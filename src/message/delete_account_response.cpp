#include "message/delete_account_response.hpp"
#include <variant>
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

DeleteAccountResponse::DeleteAccountResponse(std::variant<std::monostate, std::string> data)
    : data(data) {}

void DeleteAccountResponse::serialize(std::vector<uint8_t>& buf) const {
    buf.push_back(!is_success());
    if (std::holds_alternative<std::string>(data)) {
        std::string error = std::get<std::string>(data);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
}

void DeleteAccountResponse::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::DELETE_ACCOUNT, size());
    header.serialize(buf);
    serialize(buf);
}

void DeleteAccountResponse::deserialize(const std::vector<uint8_t>& buf) {
    if (buf[0] == 0) {
        data = std::monostate();
    } else {
        size_t len = buf[1];
        std::string error(buf.begin() + 2, buf.begin() + 2 + len);
        data = error;
    }
}

std::string DeleteAccountResponse::to_json() const {
    nlohmann::json j;
    if (is_success()) {
        j["success"] = true;
    } else {
        j["success"] = false;
        j["error"] = std::get<std::string>(data);
    }
    return j.dump();
}

void DeleteAccountResponse::from_json(const std::string& json) {
    auto j = nlohmann::json::parse(json);
    if (j["success"].get<bool>()) {
        data = std::monostate();
    } else {
        data = j["error"].get<std::string>();
    }
}

size_t DeleteAccountResponse::size() const {
    if (std::holds_alternative<std::string>(data)) {
        return 1 + (1 + std::get<std::string>(data).size());
    }
    return 1;
}

bool DeleteAccountResponse::is_success() const {
    return std::holds_alternative<std::monostate>(data);
}

std::optional<std::string> DeleteAccountResponse::get_error_message() const {
    if (std::holds_alternative<std::string>(data)) {
        return std::get<std::string>(data);
    }
    return std::nullopt;
}