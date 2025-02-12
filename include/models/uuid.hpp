#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include "message/serialize.hpp"

class UUID : public Serializable {
   public:
    UUID();

    UUID(const std::array<uint8_t, 16>& value);

    bool operator==(const UUID& other) const;

    void serialize(std::vector<uint8_t>& buf) const override;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] size_t size() const override;

    std::string to_string() const;

   private:
    std::array<uint8_t, 16> value;
};

namespace std {
template <>
struct hash<UUID> {
    std::size_t operator()(const UUID& uuid) const {
        return std::hash<std::string>{}(uuid.to_string());
    }
};
}  // namespace std