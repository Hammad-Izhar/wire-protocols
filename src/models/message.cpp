#include "models/message.hpp"

#include "models/snowflake.hpp"

Message::Message(UUID sender_id, UUID channel_id, std::string text)
    : sender_id(sender_id),
      channel_id(channel_id),
      text(text)
{

    SnowflakeIDGenerator &generator = SnowflakeIDGenerator::get_instance();

    this->snowflake = generator.nextId();
    this->created_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
    this->modified_at = this->created_at;
    this->read_by = {this->sender_id};
}

const UUID &Message::get_sender_id() const
{
    return this->sender_id;
}

const UUID &Message::get_channel_id() const
{
    return this->channel_id;
}

uint64_t Message::get_created_at() const
{
    return this->created_at;
}

uint64_t Message::get_modified_at() const
{
    return this->modified_at;
}

const std::vector<UUID> &Message::get_read_by() const
{
    return this->read_by;
}

const std::string &Message::get_text() const
{
    return this->text;
}

uint64_t Message::get_snowflake() const
{
    return this->snowflake;
}

void Message::set_text(std::string &text)
{
    this->text = text;
    this->modified_at = std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::system_clock::now().time_since_epoch())
                            .count();
}

void Message::set_read_by(UUID &user_id)
{
    this->read_by.push_back(user_id);
}
