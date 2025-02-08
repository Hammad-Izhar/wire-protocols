#include <string.h>
#include <arpa/inet.h>

#include "message/header.hpp"

Header::Header(uint8_t version, enum Operation operation, uint16_t packet_length)
    : version(version << 4 | 0x7), operation(operation), packet_length(packet_length) {}

void Header::serialize(std::vector<uint8_t> &buf) const
{
    uint16_t packet_length_be = htons(this->packet_length);

    buf[0] = this->version;
    buf[1] = static_cast<uint8_t>(this->operation);
    memcpy(&buf[2], &packet_length_be, sizeof(uint16_t));
}

void Header::deserialize(const std::vector<uint8_t> &buf)
{
    uint16_t packet_length_be;
    memcpy(&packet_length_be, &buf[2], sizeof(uint16_t));
    packet_length = ntohs(packet_length_be);

    this->version = buf[0];
    this->operation = static_cast<enum Operation>(buf[1]);
    this->packet_length = packet_length;
}

size_t Header::size() const
{
    return sizeof(version) + sizeof(operation) + sizeof(packet_length);
}

uint8_t Header::get_version() const
{
    return this->version >> 4;
}

enum Operation Header::get_operation() const
{
    return this->operation;
}

uint16_t Header::get_packet_length() const
{
    return this->packet_length;
}

void Header::set_version(uint8_t version)
{
    this->version = version << 4 | 0x7;
}

void Header::set_operation(enum Operation operation)
{
    this->operation = operation;
}

void Header::set_packet_length(uint16_t packet_length)
{
    this->packet_length = packet_length;
}