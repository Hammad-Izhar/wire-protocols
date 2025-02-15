#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include "message/serialize.hpp"

/**
 * @brief Represents a universally unique identifier (UUID).
 *
 * The UUID class encapsulates a 16-byte universally unique identifier and provides methods for
 * serialization, deserialization, and conversion to/from a string representation. It implements
 * the Serializable interface.
 */
class UUID : public Serializable {
   public:
    /**
     * @brief Default constructor.
     *
     * Initializes the UUID with a default value.
     */
    UUID();

    /**
     * @brief Constructs a new UUID with the specified value.
     *
     * @param value An array of 16 bytes representing the UUID.
     */
    UUID(const std::array<uint8_t, 16>& value);

    /**
     * @brief Equality operator.
     *
     * Compares this UUID with another for equality.
     *
     * @param other The other UUID to compare against.
     * @return true if both UUIDs are equal; false otherwise.
     */
    bool operator==(const UUID& other) const;

    /**
     * @brief Serializes the UUID into a byte buffer.
     *
     * Appends the 16-byte UUID representation to the provided buffer.
     *
     * @param buf The byte buffer where the UUID data will be appended.
     */
    void serialize(std::vector<uint8_t>& buf) const override;

    /**
     * @brief Deserializes the UUID from a byte buffer.
     *
     * Reads 16 bytes from the provided buffer to set the UUID value.
     *
     * @param buf The byte buffer containing the serialized UUID data.
     */
    void deserialize(const std::vector<uint8_t>& buf) override;

    /**
     * @brief Gets the size of the serialized UUID.
     *
     * @return The size in bytes of the serialized UUID (always 16).
     */
    [[nodiscard]] size_t size() const override;

    /**
     * @brief Converts the UUID to its string representation.
     *
     * @return A string representation of the UUID.
     */
    std::string to_string() const;

    /**
     * @brief Creates a UUID from its string representation.
     *
     * Parses the given string and returns the corresponding UUID.
     *
     * @param str The string representation of a UUID.
     * @return The UUID constructed from the string.
     */
    static UUID from_string(const std::string& str);

   private:
    /// The 16-byte array that stores the UUID value.
    std::array<uint8_t, 16> value;
};

namespace std {

/**
 * @brief Specialization of std::hash for UUID.
 *
 * Allows UUID objects to be used in unordered containers by hashing their string representation.
 */
template <>
struct hash<UUID> {
    /**
     * @brief Hashes a UUID.
     *
     * @param uuid The UUID to hash.
     * @return A hash value computed from the UUID's string representation.
     */
    std::size_t operator()(const UUID& uuid) const {
        return std::hash<std::string>{}(uuid.to_string());
    }
};

}  // namespace std
