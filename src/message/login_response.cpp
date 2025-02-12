#include "message/login_response.hpp"
#include "message/header.hpp"
#include "constants.hpp"

LoginResponse::LoginResponse(std::variant<std::monostate, std::string> error_message) : error_message(std::move(error_message)) {}

void LoginResponse::serialize(std::vector<uint8_t> &buf) const
{
    if (std::holds_alternative<std::monostate>(error_message))
    {
        buf.push_back(0);
    }
    else
    {
        buf.push_back(1);
        const std::string &error = std::get<std::string>(error_message);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
}

void LoginResponse::serialize_msg(std::vector<uint8_t> &buf) const
{
    Header header(PROTOCOL_VERSION, Operation::LOGIN, this->size());
    header.serialize(buf);
    serialize(buf);
}

void LoginResponse::deserialize(const std::vector<uint8_t> &buf)
{
    size_t offset = 0;
    uint8_t has_error = buf[offset++];
    if (has_error == 0)
    {
        error_message = std::monostate();
    }
    else
    {
        uint8_t error_length = buf[offset++];
        std::string error(buf.begin() + offset, buf.begin() + offset + error_length);
        error_message = error;
    }
}

[[nodiscard]] size_t LoginResponse::size() const
{
    size_t size = 1; // for the has_error byte
    if (std::holds_alternative<std::string>(error_message))
    {
        const std::string &error = std::get<std::string>(error_message);
        size += 1 + error.size(); // 1 for the error length + error length
    }
    return size;
}

[[nodiscard]] bool LoginResponse::is_success() const
{
    return std::holds_alternative<std::monostate>(error_message);
}

[[nodiscard]] const std::optional<std::string> LoginResponse::get_error_message() const
{
    if (is_success())
    {
        return std::nullopt;
    }
    else
    {
        return std::get<std::string>(error_message);
    }
}