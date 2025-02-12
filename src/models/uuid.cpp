#include <random>
#include <functional>
#include <iostream>

#include "models/uuid.hpp"

UUID::UUID()
{
    // Generate a random UUID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);

    for (auto &byte : this->value)
    {
        byte = dis(gen);
    }
}

void UUID::serialize(std::vector<uint8_t> &buf) const
{
    for (auto byte : this->value)
    {
        buf.push_back(byte);
    }
}

void UUID::deserialize(const std::vector<uint8_t> &buf)
{
    for (int i = 0; i < 16; i++)
    {
        this->value[i] = buf[i];
    }
}

bool UUID::operator==(const UUID &other) const
{
    return this->value == other.value;
}

std::string UUID::toString() const
{
    std::string result;
    for (auto byte : this->value)
    {
        result += (byte < 16 ? "0" : "") + std::to_string(byte);
    }
    return result;
}