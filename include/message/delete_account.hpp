#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"

class DeleteAccountMessage : public Serializable {
   public:
    DeleteAccountMessage() = default;

    DeleteAccountMessage(std::string username, std::string password);

    void serialize(std::vector<uint8_t>& buf) const override;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] std::string to_json() const;

    void from_json(const std::string& json);

    [[nodiscard]] size_t size() const override;

    [[nodiscard]] const std::string& get_username() const;

    [[nodiscard]] const std::string& get_password() const;

    void set_username(const std::string& username);

    void set_password(const std::string& password);

   private:
    std::string username;
    std::string password;
};