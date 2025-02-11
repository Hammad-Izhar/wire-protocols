#include "models/message.hpp"
#include <random>


Message::Message(std::array<uint8_t,16> sender_id, std::array<uint8_t,16> channel_id, std::string text) :
    sender_id(sender_id), channel_id(channel_id), text(std::move(text))
{
    // Generate a random snowflake
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

    this->snowflake = dis(gen);
    this->created_at = time(nullptr);
    this->modified_at = this->created_at;
    this->read_by = {this->sender_id};
}

const std::array<uint8_t,16> &Message::get_sender_id() const
{
    return this->sender_id;
}

const std::array<uint8_t,16> &Message::get_channel_id() const
{
    return this->channel_id;
}

time_t Message::get_created_at() const
{
    return this->created_at;
}

time_t Message::get_modified_at() const
{
    return this->modified_at;
}

const std::vector<std::array<uint8_t,16>> &Message::get_read_by() const
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
    this->modified_at = time(nullptr);
}

void Message::set_read_by(std::array<uint8_t,16> &user_id)
{
    this->read_by.push_back(user_id);
}


