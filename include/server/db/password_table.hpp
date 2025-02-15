#pragma once
#include <stdint.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

#include "models/uuid.hpp"

/**
 * @brief Manages user passwords with secure storage and verification.
 *
 * The PasswordTable class is responsible for storing and managing passwords associated with user UUIDs.
 * It provides methods to verify, add, and remove passwords. Passwords are stored in a hashed form along
 * with a salt for added security. The class ensures thread-safe access using a mutex.
 */
class PasswordTable {
   public:
    /**
     * @brief Default constructor.
     *
     * Constructs a new PasswordTable with an empty password storage.
     */
    PasswordTable() = default;

    /**
     * @brief Verifies the password for a given user.
     *
     * Checks if the provided password, when hashed with the stored salt, matches the stored hashed password.
     *
     * @param user_uid A reference to the UUID of the user.
     * @param password The password to verify.
     * @return A variant containing a boolean indicating success (true) or failure (false),
     *         or an error message string if verification could not be performed.
     */
    std::variant<bool, std::string> verify_password(UUID& user_uid, std::string password);

    // Setters

    /**
     * @brief Adds a password for a user.
     *
     * Hashes the provided password with a generated salt and stores the result in the table.
     *
     * @param user_uid A reference to the UUID of the user.
     * @param password The password to add.
     * @return A variant containing std::monostate on success, or an error message string on failure.
     */
    std::variant<std::monostate, std::string> add_password(UUID& user_uid, std::string password);

    /**
     * @brief Removes a user's password.
     *
     * Deletes the password entry associated with the given user UUID.
     *
     * @param user_uid A reference to the UUID of the user whose password is to be removed.
     * @return A variant containing std::monostate on success, or an error message string on failure.
     */
    std::variant<std::monostate, std::string> remove_password(UUID& user_uid);

   private:
    /// Maps a user's UUID to a pair containing the hashed password and its associated salt.
    std::unordered_map<UUID, std::pair<std::string, std::string>> data;
    /// Mutex to ensure thread-safe access to the password table.
    std::mutex mutex;

    /**
     * @brief Computes the SHA-256 hash of a given string.
     *
     * @param str The input string to hash.
     * @return The resulting SHA-256 hash as a string.
     */
    std::string sha256(const std::string& str);

    /**
     * @brief Generates a random salt for password hashing.
     *
     * @return A randomly generated salt string.
     */
    std::string generate_salt();
};
