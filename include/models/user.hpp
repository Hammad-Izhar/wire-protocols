#pragma once
#include <stdint.h>
#include <vector>
#include <array>
#include <string>
#include <algorithm>

class User
{
public:
    User(std::string username, std::string password, std::string display_name);

    // Getters
    [[nodiscard]] const std::array<uint8_t,16> &get_uid() const;

    [[nodiscard]] const std::string &get_username() const;

    [[nodiscard]] const std::string &get_display_name() const;

    [[nodiscard]] const std::string &get_profile_pic() const;

    // [[nodiscard]] const std::string &get_public_key() const;

    [[nodiscard]] const std::vector< std::array<uint8_t,16> > &get_channels() const;

    // [[nodiscard]] const std::vector<std::array<uint8_t,16> > &get_channels() const;

    // Setters

    void set_username(std::string username);

    void set_display_name(std::string display_name);

    void set_profile_pic(std::string profile_pic);

    void add_channel(std::array<uint8_t,16> channel);

    void remove_channel(std::array<uint8_t,16> channel);

private:
    std::array<uint8_t,16> uid; // User ID
    std::string username; // Username
    std::string password; // Password
    std::string display_name; // Display name
    std::string profile_pic; // URL to profile picture
    std::string public_key; // Public key for encryption
    std::vector< std::array<uint8_t,16> > channels; // List of channel IDs
};