#pragma once
#include <stdint.h>
#include <vector>
#include <array>
#include <string>
#include <algorithm>

#include "models/uuid.hpp"

class User
{
public:
    User(std::string username, std::string display_name);

    // Getters
    [[nodiscard]] const UUID &get_uid() const;

    [[nodiscard]] const std::string &get_username() const;

    [[nodiscard]] const std::string &get_display_name() const;

    [[nodiscard]] const std::string &get_profile_pic() const;

    // [[nodiscard]] const std::string &get_public_key() const;

    [[nodiscard]] const std::vector<UUID> &get_channels() const;

    // Setters

    void set_username(std::string username);

    void set_display_name(std::string display_name);

    void set_profile_pic(std::string profile_pic);

    void add_channel(UUID channel);

    void remove_channel(UUID channel_id);

private:
    UUID uid;                   // User ID
    std::string username;       // Username
    std::string display_name;   // Display name
    std::string profile_pic;    // URL to profile picture
    std::string public_key;     // Public key for encryption
    std::vector<UUID> channels; // List of channel IDs
};