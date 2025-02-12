#include "message/list_accounts.hpp"
#include "message/header.hpp"
#include "constants.hpp"
#include <stdint.h>
#include <string>

ListAccountsMessage::ListAccountsMessage(std::string regex) : regex(std::move(regex)) {}

void ListAccountsMessage::serialize(std::vector<uint8_t> &buf) const
{
    uint8_t regex_length = this->regex.size();

    buf.push_back(regex_length);
    for (const auto &c : this->regex)
    {
        buf.push_back(c);
    }
}

void ListAccountsMessage::serialize_msg(std::vector<uint8_t> &buf) const
{
    Header header(PROTOCOL_VERSION, Operation::LIST_ACCOUNTS, this->size());
    header.serialize(buf);
    this->serialize(buf);
}

void ListAccountsMessage::deserialize(const std::vector<uint8_t> &buf)
{
    uint8_t regex_length = buf[0];

    this->regex = std::string(buf.begin() + 1, buf.begin() + 1 + regex_length);
}

size_t ListAccountsMessage::size() const
{
    return 1 + this->regex.size();
}

std::string ListAccountsMessage::get_regex() const
{
    return this->regex;
}

void ListAccountsMessage::set_regex(std::string regex)
{
    this->regex = std::move(regex);
}
