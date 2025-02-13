#include "message/list_accounts_response.hpp"
#include <memory>
#include "constants.hpp"
#include "message/header.hpp"

ListAccountsResponse::ListAccountsResponse(std::vector<User::SharedPtr> data)
    : data(std::move(data)) {}

void ListAccountsResponse::serialize(std::vector<uint8_t>& buf) const {
    if (std::holds_alternative<std::vector<User::SharedPtr>>(data)) {
        buf.push_back(0);
        // Push back length of vector
        std::vector<User::SharedPtr> users = std::get<std::vector<User::SharedPtr>>(data);
        buf.push_back(users.size());
        for (const auto& user : users) {
            user->serialize(buf);
        }
    } else {
        buf.push_back(1);
        const std::string& error = std::get<std::string>(data);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
}

void ListAccountsResponse::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::LIST_ACCOUNTS, this->size());
    header.serialize(buf);
    serialize(buf);
}

void ListAccountsResponse::deserialize(const std::vector<uint8_t>& buf) {
    size_t offset = 0;
    uint8_t has_error = buf[offset++];
    if (has_error == 0) {
        std::vector<User::SharedPtr> users = {};
        uint8_t users_length = buf[offset++];
        for (int i = 0; i < users_length; i++) {
            User::SharedPtr user = std::make_shared<User>();
            user->deserialize(std::vector<uint8_t>(buf.begin() + offset, buf.end()));
            offset += user->size();
            users.push_back(user);
        }
        data = users;
    } else {
        uint8_t error_length = buf[offset++];
        std::string error(buf.begin() + offset, buf.begin() + offset + error_length);
        data = error;
    }
}

[[nodiscard]] size_t ListAccountsResponse::size() const {
    size_t size = 1;  // for the has_error byte
    if (std::holds_alternative<std::vector<User::SharedPtr>>(data)) {
        size += 1;  // for the users length
        std::vector<User::SharedPtr> users = std::get<std::vector<User::SharedPtr>>(data);
        for (const auto& user : users) {
            size += user->size();
        }
    } else {
        const std::string& error = std::get<std::string>(data);
        size += 1 + error.size();  // 1 for the error length + error length
    }
    return size;
}

[[nodiscard]] bool ListAccountsResponse::is_success() const {
    return std::holds_alternative<std::vector<User::SharedPtr>>(data);
}

std::optional<std::vector<User::SharedPtr>> ListAccountsResponse::get_users() {
    if (std::holds_alternative<std::vector<User::SharedPtr>>(data)) {
        return std::get<std::vector<User::SharedPtr>>(data);
    }
    return std::nullopt;
}

std::optional<std::string> ListAccountsResponse::get_error_message() const {
    if (std::holds_alternative<std::string>(data)) {
        return std::get<std::string>(data);
    }
    return std::nullopt;
}