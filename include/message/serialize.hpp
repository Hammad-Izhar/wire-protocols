#pragma once
#include <stdint.h>
#include <cstddef>
#include <vector>

class Serializable {
   public:
    virtual ~Serializable() = default;

    virtual void serialize(std::vector<uint8_t>& buf) const = 0;

    virtual void deserialize(const std::vector<uint8_t>& buf) = 0;

    virtual size_t size() const = 0;
};