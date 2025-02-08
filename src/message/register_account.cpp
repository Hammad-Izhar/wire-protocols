#include "message/register_account.hpp"
#include "message/header.hpp"
#include "constants.hpp"

RegisterAccountMessage::RegisterAccountMessage(std::string username, std::string password, std::string display_name)
    : username(username), password(password), display_name(display_name) {}

void RegisterAccountMessage::serialize(std::vector<uint8_t> &buf) const
{

    uint8_t username_length = this->username.size();
    uint8_t password_length = this->password.size();
    uint8_t display_name_length = this->display_name.size();

    buf.push_back(username_length);
    for (const auto &c : this->username)
    {
        buf.push_back(c);
    }

    buf.push_back(password_length);
    for (const auto &c : this->password)
    {
        buf.push_back(c);
    }

    buf.push_back(display_name_length);
    for (const auto &c : this->display_name)
    {
        buf.push_back(c);
    }
}

void RegisterAccountMessage::serialize_msg(std::vector<uint8_t> &buf) const
{
    Header header(PROTOCOL_VERSION, Operation::REGISTER_ACCOUNT, this->size());
    header.serialize(buf);
    this->serialize(buf);
}

void RegisterAccountMessage::deserialize(const std::vector<uint8_t> &buf)
{
    uint8_t username_length = buf[0];
    uint8_t password_length = buf[username_length + 1];
    uint8_t display_name_length = buf[username_length + password_length + 2];

    this->username = std::string(buf.begin() + 1, buf.begin() + 1 + username_length);
    this->password = std::string(buf.begin() + username_length + 2, buf.begin() + username_length + 2 + password_length);
    this->display_name = std::string(buf.begin() + username_length + password_length + 3, buf.begin() + username_length + password_length + 3 + display_name_length);
}

size_t RegisterAccountMessage::size() const
{
    return 3 + this->username.size() + this->password.size() + this->display_name.size();
}

std::string RegisterAccountMessage::get_username() const
{
    return this->username;
}

std::string RegisterAccountMessage::get_password() const
{
    return this->password;
}

std::string RegisterAccountMessage::get_display_name() const
{
    return this->display_name;
}

void RegisterAccountMessage::set_username(std::string username)
{
    this->username = username;
}

void RegisterAccountMessage::set_password(std::string password)
{
    this->password = password;
}

void RegisterAccountMessage::set_display_name(std::string display_name)
{
    this->display_name = display_name;
}
