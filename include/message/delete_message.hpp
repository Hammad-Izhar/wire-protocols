#pragma once
#include <stdint.h>

#include "message/serialize.hpp"
#include "models/uuid.hpp"

class DeleteMessageMessage : public Serializable {
   public:
    DeleteMessageMessage() = default;

    DeleteMessageMessage(UUID channel_uid, uint64_t message_snowflake);

    void serialize(std::vector<uint8_t>& buf) const override;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf) override;

    [[nodiscard]] std::string to_json() const;

    void from_json(const std::string& json);

    [[nodiscard]] uint64_t get_message_snowflake() const;

    [[nodiscard]] size_t size() const override;

   private:
    UUID channel_uid;
    uint64_t message_snowflake;
};