#include "message/list_accounts.hpp"
#include <stdint.h>
#include <string>
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

ListAccountsMessage::ListAccountsMessage(std::string regex) : regex(std::move(regex)) {}

void ListAccountsMessage::serialize(std::vector<uint8_t>& buf) const {
#if PROTOCOL_JSON
    std::string msg = this->to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    uint8_t regex_length = this->regex.size();

    buf.push_back(regex_length);
    for (const auto& c : this->regex) {
        buf.push_back(c);
    }
#endif
}

void ListAccountsMessage::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::LIST_ACCOUNTS, this->size());
    header.serialize(buf);
    this->serialize(buf);
}

void ListAccountsMessage::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    std::string msg(buf.begin(), buf.end());
    this->from_json(msg);
#else
    uint8_t regex_length = buf[0];

    this->regex = std::string(buf.begin() + 1, buf.begin() + 1 + regex_length);
#endif
}

std::string ListAccountsMessage::to_json() const {
    nlohmann::json j;
    j["regex"] = this->regex;
    return j.dump();
}

void ListAccountsMessage::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    this->regex = j["regex"];
}

size_t ListAccountsMessage::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    return 1 + this->regex.size();
#endif
}

std::string ListAccountsMessage::get_regex() const {
    return this->regex;
}

void ListAccountsMessage::set_regex(std::string regex) {
    this->regex = std::move(regex);
}
