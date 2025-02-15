#pragma once
#include <stdint.h>
#include <cstddef>
#include <vector>

/**
 * @brief An abstract interface for serializable objects.
 *
 * This class defines the interface for objects that can be serialized to and deserialized from a byte buffer.
 * All classes inheriting from Serializable must implement methods for serialization, deserialization,
 * and for obtaining the size of the serialized data.
 */
class Serializable {
   public:
    /**
     * @brief Virtual destructor.
     *
     * Ensures that derived class destructors are called correctly when an object is deleted via a pointer
     * to Serializable.
     */
    virtual ~Serializable() = default;

    /**
     * @brief Serializes the object into a byte buffer.
     *
     * Converts the current state of the object into a sequence of bytes and appends the data to the provided buffer.
     *
     * @param buf The byte buffer where the serialized data will be appended.
     */
    virtual void serialize(std::vector<uint8_t>& buf) const = 0;

    /**
     * @brief Deserializes the object from a byte buffer.
     *
     * Reads data from the provided byte buffer and restores the object's state.
     *
     * @param buf The byte buffer containing the serialized data.
     */
    virtual void deserialize(const std::vector<uint8_t>& buf) = 0;

    /**
     * @brief Gets the size of the serialized object.
     *
     * Calculates and returns the number of bytes that would be produced by serializing the object.
     *
     * @return The size in bytes of the serialized object.
     */
    virtual size_t size() const = 0;
};
