#pragma once
#include <stdint.h>
#include <cstdint>

/**
 * @file protocol_version.hpp
 * @brief Defines protocol version constants used in the application.
 *
 * This file specifies the protocol version numbers for different communication
 * formats. Two protocol versions are defined:
 * - A custom binary protocol version.
 * - A JSON-based protocol version.
 * The active protocol version is selected at compile time based on whether the
 * PROTOCOL_JSON macro is defined.
 */

/**
 * @brief Custom protocol version.
 *
 * This version is used when the custom binary protocol is employed.
 */
constexpr uint8_t PROTOCOL_VERSION_CUSTOM = 1;

/**
 * @brief JSON protocol version.
 *
 * This version is used when the JSON protocol is employed.
 */
constexpr uint8_t PROTOCOL_VERSION_JSON = 2;

#ifdef PROTOCOL_JSON
/**
 * @brief The active protocol version.
 *
 * When the PROTOCOL_JSON macro is defined, the active protocol version is set
 * to the JSON protocol version.
 */
constexpr uint8_t PROTOCOL_VERSION = PROTOCOL_VERSION_JSON;
#else
/**
 * @brief The active protocol version.
 *
 * When the PROTOCOL_JSON macro is not defined, the active protocol version is set
 * to the custom binary protocol version.
 */
constexpr uint8_t PROTOCOL_VERSION = PROTOCOL_VERSION_CUSTOM;
#endif
