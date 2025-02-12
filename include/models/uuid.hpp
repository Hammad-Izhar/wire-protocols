#pragma once
#include <array>
#include <stdint.h>
#include <string>
#include "message/serialize.hpp"

class UUID : public Serializable
{
public:
    // Define the UUID as a 16-byte array
    std::array<uint8_t, 16> value;

    // Default constructor (optional)
    UUID();

    void serialize(std::vector<uint8_t> &buf) const override;
    void deserialize(const std::vector<uint8_t> &buf) override;

    // Comparison operator for equality
    bool operator==(const UUID &other) const;

    // Method to print the uuid (for testing/debugging)
    std::string toString() const;
};

namespace std
{
    template <>
    struct hash<UUID>
    {
        std::size_t operator()(const UUID &uuid) const
        {
            // Implement a hash function for UUID
            // For example, if UUID is a struct with a single member variable:
            return std::hash<std::string>{}(uuid.toString()); // Assuming UUID has a toString() method
        }
    };
}