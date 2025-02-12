#pragma once
#include <stdint.h>
#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

class User : public Serializable {
   public:
    typedef std::shared_ptr<User> SharedPtr;

    User(std::string username, std::string display_name);
    User(std::string username, std::string display_name, UUID uid, std::string profile_pic);
    User() = default;

    void serialize(std::vector<uint8_t>& buf) const override;
    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] size_t size() const;

    // Getters
    [[nodiscard]] const UUID& get_uid();

    [[nodiscard]] const std::string& get_username();

    [[nodiscard]] const std::string& get_display_name();

    [[nodiscard]] const std::string& get_profile_pic();

    // [[nodiscard]] const std::string &get_public_key() ;

    [[nodiscard]] const std::vector<UUID>& get_channels();

    // Setters

    void set_username(std::string username);

    void set_display_name(std::string display_name);

    void set_profile_pic(std::string profile_pic);

    void add_channel(UUID channel);

    void remove_channel(UUID channel_id);

   private:
    UUID uid;                    // User ID
    std::string username;        // Username
    std::string display_name;    // Display name
    std::string profile_pic;     // URL to profile picture
    std::string public_key;      // Public key for encryption
    std::vector<UUID> channels;  // List of channel IDs
    std::mutex mutex;
};