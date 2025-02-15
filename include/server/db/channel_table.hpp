#pragma once
#include <stdint.h>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>

#include "models/channel.hpp"
#include "models/uuid.hpp"

/**
 * @brief Manages a collection of channels.
 *
 * The ChannelTable class provides a thread-safe interface for storing and managing channels identified by their unique UUIDs.
 * It offers methods for retrieving channels in both read-only and mutable forms, as well as methods for adding and removing channels.
 */
class ChannelTable {
   public:
    /**
     * @brief Default constructor.
     *
     * Constructs a new ChannelTable instance with an empty collection of channels.
     */
    ChannelTable() = default;

    /**
     * @brief Retrieves a channel by its unique identifier (read-only).
     *
     * Searches the channel table for a channel with the specified UUID.
     *
     * @param channel_uid The UUID of the channel to retrieve.
     * @return An optional containing a constant shared pointer to the channel if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<const Channel::SharedPtr> get_by_uid(UUID channel_uid);

    /**
     * @brief Retrieves a channel by its unique identifier (mutable).
     *
     * Searches the channel table for a channel with the specified UUID and returns a mutable shared pointer.
     *
     * @param channel_uid The UUID of the channel to retrieve.
     * @return An optional containing a mutable shared pointer to the channel if found, or std::nullopt otherwise.
     */
    [[nodiscard]] std::optional<Channel::SharedPtr> get_mut_by_uid(UUID channel_uid);

    /**
     * @brief Adds a new channel to the table.
     *
     * Creates and stores a new channel with the specified name and initial members.
     * On success, returns a shared pointer to the newly created channel.
     * On failure, returns a string describing the error.
     *
     * @param channel_name The name of the channel to add.
     * @param members A vector of UUIDs representing the initial members of the channel.
     * @return A variant containing either a shared pointer to the new channel on success, or an error message string on failure.
     */
    std::variant<Channel::SharedPtr, std::string> add_channel(std::string channel_name,
                                                              std::vector<UUID> members);

    /**
     * @brief Removes a channel from the table.
     *
     * Deletes the channel identified by the specified UUID from the table.
     * On success, returns std::monostate.
     * On failure, returns a string describing the error.
     *
     * @param channel_uid The UUID of the channel to remove.
     * @return A variant containing std::monostate on success or an error message string on failure.
     */
    std::variant<std::monostate, std::string> remove_channel(UUID channel_uid);

   private:
    /// Maps channel UUIDs to their corresponding shared pointers.
    std::unordered_map<UUID, Channel::SharedPtr> data;
    /// Mutex to ensure thread-safe access to the channel table.
    std::mutex mutex;
};
