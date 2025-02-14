#include <cstring>
#include <iomanip>
#include <random>
#include <sstream>

#include <QDebug>
#include "models/uuid.hpp"

UUID::UUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint8_t> dis(0, 255);

    for (auto& byte : this->value) {
        byte = dis(gen);
    }
}

UUID::UUID(const std::array<uint8_t, 16>& value) : value(value) {}

bool UUID::operator==(const UUID& other) const {
    return this->value == other.value;
}

void UUID::serialize(std::vector<uint8_t>& buf) const {
    buf.insert(buf.end(), this->value.begin(), this->value.end());
}

void UUID::deserialize(const std::vector<uint8_t>& buf) {
    std::memcpy(this->value.data(), buf.data(), UUID::size());
};

size_t UUID::size() const {
    return this->value.size();
}

std::string UUID::to_string() const {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');

    for (uint8_t byte : this->value) {
        ss << std::setw(2) << static_cast<int>(byte);
    }

    std::string hex_str = ss.str();

    return hex_str;
}

UUID UUID::from_string(const std::string& str) {
    UUID uuid;  // Creates a new UUID instance

    qDebug() << "Parsing UUID from string: " << str.c_str();

    for (size_t i = 0; i < str.size(); i += 2) {
        std::string byteString = str.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
        uuid.value[i / 2] = byte;
    }

    qDebug() << "Parsed UUID: " << uuid.to_string().c_str();

    return uuid;
}
