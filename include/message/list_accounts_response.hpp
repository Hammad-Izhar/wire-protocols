#pragma once
#include <stdint.h>
#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/user.hpp"

class ListAccountsResponse : public Serializable {
   public:
    ListAccountsResponse() = default;

    ListAccountsResponse(std::vector<User::SharedPtr> data);

    void serialize(std::vector<uint8_t>& buf) const override;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] size_t size() const override;

    [[nodiscard]] bool is_success() const;

    [[nodiscard]] std::optional<std::string> get_error_message() const;

    [[nodiscard]] std::optional<std::vector<User::SharedPtr>> get_users();

   private:
    std::variant<std::vector<User::SharedPtr>, std::string> data;
};