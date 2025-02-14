#pragma once
#include <stdint.h>
#include <memory>
#include <mutex>
#include <string>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

class Channel : public Serializable {
   public:
    typedef std::shared_ptr<Channel> SharedPtr;

    Channel() = default;

    Channel(std::string name, std::vector<UUID> user_uids);

    void serialize(std::vector<uint8_t>& buf) const override;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] size_t size() const override;

    // Getters
    [[nodiscard]] const UUID& get_uid();
    [[nodiscard]] const std::string& get_name();
    [[nodiscard]] const std::vector<UUID>& get_user_uids();
    [[nodiscard]] const std::vector<uint64_t>& get_message_snowflakes();

    // Setters
    void set_name(std::string name);
    void add_user(UUID user_uid);
    void add_message(const uint64_t& message_snowflake);
    void remove_user(const UUID& user_uid);
    void remove_message(const uint64_t& message_snowflake);

   private:
    UUID uid;
    std::string name;
    std::vector<UUID> user_uids;
    std::vector<uint64_t> message_snowflakes;
    std::mutex mutex;
};