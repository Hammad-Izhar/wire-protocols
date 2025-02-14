#pragma once
#include <stdint.h>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

class Message : public Serializable {
   public:
    typedef std::shared_ptr<Message> SharedPtr;

    Message(UUID sender_id, UUID channel_id, std::string text);

    Message() = default;

    void serialize(std::vector<uint8_t>& buf) const override;
    void serialize_msg(std::vector<uint8_t>& buf) const;
    void deserialize(const std::vector<uint8_t>& buf) override;
    [[nodiscard]] size_t size() const override;

    // Getters
    [[nodiscard]] const uint64_t get_snowflake();
    [[nodiscard]] const UUID& get_sender_id();
    [[nodiscard]] const UUID& get_channel_id();
    [[nodiscard]] const uint64_t get_created_at();
    [[nodiscard]] const uint64_t get_modified_at();
    [[nodiscard]] const std::vector<UUID>& get_read_by();
    [[nodiscard]] const std::string& get_text();

    // Setters
    void set_text(std::string& text);
    void set_read_by(UUID& user_id);

   private:
    uint64_t snowflake;
    UUID sender_id;
    UUID channel_id;
    uint64_t created_at;
    uint64_t modified_at;
    std::vector<UUID> read_by;
    std::string text;
    std::mutex mutex;
};