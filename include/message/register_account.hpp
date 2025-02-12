#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"

class RegisterAccountMessage : public Serializable
{
public:
    RegisterAccountMessage() = default;

    RegisterAccountMessage(std::string username, std::string password, std::string display_name);

    void serialize(std::vector<uint8_t> &buf) const override;

    void serialize_msg(std::vector<uint8_t> &buf) const;

    void deserialize(const std::vector<uint8_t> &buf) override;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] const std::string &get_username() const;

    [[nodiscard]] const std::string &get_password() const;

    [[nodiscard]] const std::string &get_display_name() const;

    void set_username(std::string username);

    void set_password(std::string password);

    void set_display_name(std::string display_name);

private:
    std::string username;
    std::string password;
    std::string display_name;
};