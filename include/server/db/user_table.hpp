#pragma once
#include <mutex>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>

#include "models/user.hpp"
#include "models/uuid.hpp"

/**
 * @brief Manages a collection of users.
 *
 * The UserTable class provides a thread-safe interface for storing and managing users
 * identified by their unique UUIDs. It supports retrieving users (both read-only and mutable),
 * searching for user UUIDs that match a regular expression, and adding or removing users.
 */
class UserTable {
   public:
    /**
     * @brief Default constructor.
     *
     * Constructs a new UserTable instance with an empty collection of users.
     */
    UserTable() = default;

    /**
     * @brief Retrieves a user by their unique identifier (read-only).
     *
     * Searches the user table for a user with the specified UUID.
     *
     * @param user_uid The UUID of the user to retrieve.
     * @return An optional containing a constant shared pointer to the user if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<const User::SharedPtr> get_by_uid(UUID user_uid);

    /**
     * @brief Retrieves a user by their unique identifier (mutable).
     *
     * Searches the user table for a user with the specified UUID and returns a mutable shared pointer.
     *
     * @param user_uid The UUID of the user to retrieve.
     * @return An optional containing a mutable shared pointer to the user if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<User::SharedPtr> get_mut_by_uid(UUID user_uid);

    /**
     * @brief Retrieves UUIDs of users matching a given regular expression.
     *
     * Searches the user table and returns a vector of UUIDs for users whose associated
     * data matches the specified regex.
     *
     * @param regex The regular expression to match against.
     * @return A variant containing a vector of matching UUIDs on success, or an error message string on failure.
     */
    [[nodiscard]] std::variant<std::vector<UUID>, std::string> get_uuids_matching_regex(std::string regex);

    /**
     * @brief Retrieves a user's UUID from their username.
     *
     * Searches the user table for a user with the specified username and returns their UUID.
     *
     * @param username The username of the user.
     * @return An optional containing the user's UUID if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<UUID> get_uid_from_username(std::string username);

    /**
     * @brief Adds a new user to the table.
     *
     * Inserts the provided user into the table.
     *
     * @param user A shared pointer to the User to add.
     * @return A variant containing std::monostate on success, or an error message string on failure.
     */
    std::variant<std::monostate, std::string> add_user(User::SharedPtr user);

    /**
     * @brief Removes a user from the table.
     *
     * Deletes the user with the specified UUID from the table.
     *
     * @param user_uid The UUID of the user to remove.
     * @return A variant containing a shared pointer to the removed User on success, or an error message string on failure.
     */
    std::variant<User::SharedPtr, std::string> remove_user(UUID user_uid);

   private:
    /// Maps user UUIDs to their corresponding shared pointers.
    std::unordered_map<UUID, User::SharedPtr> data;
    /// Mutex to ensure thread-safe access to the user table.
    std::mutex mutex;
};
