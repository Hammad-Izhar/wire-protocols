#pragma once
#include <stdint.h>
#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/user.hpp"

class LoginResponse : public Serializable {
   public:
    LoginResponse() = default;

    LoginResponse(std::variant<User::SharedPtr, std::string> data);

    void serialize(std::vector<uint8_t>& buf) const override;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] bool is_success() const;

    [[nodiscard]] const std::optional<User::SharedPtr> get_data() const;

    [[nodiscard]] const std::optional<std::string> get_error_message() const;

   private:
    std::variant<User::SharedPtr, std::string> data;
};