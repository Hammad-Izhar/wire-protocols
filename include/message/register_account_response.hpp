#pragma once
#include <stdint.h>
#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"

class RegisterAccountResponse : public Serializable {
   public:
    RegisterAccountResponse() = default;

    RegisterAccountResponse(std::variant<std::monostate, std::string> error_message);

    void serialize(std::vector<uint8_t>& buf) const override;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] std::string to_json() const;

    void from_json(const std::string& json);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] bool is_success() const;

    [[nodiscard]] const std::optional<std::string> get_error_message() const;

   private:
    std::variant<std::monostate, std::string> error_message;
};