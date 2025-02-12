#pragma once
#include <stdint.h>
#include <string>

#include "message/serialize.hpp"

class ListAccountsMessage : public Serializable
{
public:
    ListAccountsMessage() = default;

    ListAccountsMessage(std::string regex);

    void serialize(std::vector<uint8_t> &buf) const override;

    void serialize_msg(std::vector<uint8_t> &buf) const;

    void deserialize(const std::vector<uint8_t> &buf) override;

    [[nodiscard]] size_t size() const;

    void set_regex(std::string regex);

private:
    std::string regex;
};