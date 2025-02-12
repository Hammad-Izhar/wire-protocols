#pragma once
#include <stdint.h>
#include <span>
#include <vector>

class Serializable {
   public:
    virtual ~Serializable() = default;

    virtual void serialize(std::vector<uint8_t>& buf) const = 0;
    virtual void deserialize(const std::vector<uint8_t>& buf) = 0;
};