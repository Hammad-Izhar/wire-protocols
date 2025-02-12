#include "models/UUID.hpp"
#include <random>
#include <functional>
#include <iostream>

UUID::UUID() {
    // Generate a random UUID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);

    for (auto& byte : this->value) {
        byte = dis(gen);
    }
}

bool UUID::operator==(const UUID& other) const {
    return this->value == other.value;
}

std::string UUID::toString() const {
    std::string result;
    for (auto byte : this->value) {
        result += (byte < 16 ? "0" : "") + std::to_string(byte);
    }
    return result;
}

void UUID::print() const {
    std::cout << this->toString() << std::endl;
}