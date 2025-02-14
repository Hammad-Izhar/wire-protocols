#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"

class ListAccountsMessage : public Serializable {
   public:
    ListAccountsMessage() = default;

    ListAccountsMessage(std::string regex);

    void serialize(std::vector<uint8_t>& buf) const override;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] std::string to_json() const;

    void from_json(const std::string& json);

    [[nodiscard]] std::string get_regex() const;

    [[nodiscard]] size_t size() const override;

    void set_regex(std::string regex);

   private:
    std::string regex;
};