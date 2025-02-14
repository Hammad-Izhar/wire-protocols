#pragma once
#include <stdint.h>
#include <cstdint>

constexpr uint8_t PROTOCOL_VERSION_CUSTOM = 1;

constexpr uint8_t PROTOCOL_VERSION_JSON = 2;

#ifdef PROTOCOL_JSON
constexpr uint8_t PROTOCOL_VERSION = PROTOCOL_VERSION_JSON;
#else
constexpr uint8_t PROTOCOL_VERSION = PROTOCOL_VERSION_CUSTOM;
#endif