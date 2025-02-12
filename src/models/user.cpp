#include <random>
#include <stdexcept>

#include "models/user.hpp"

User::User(std::string username, std::string display_name)
    : username(username),
      display_name(display_name),
      uid(UUID()),
      profile_pic("./assets/profile_pics/blank_profile_pic.png")
{
    // Generate a public/private key pair
    // TBD to friday
}

const std::string &User::get_username()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->username;
}

const UUID &User::get_uid()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->uid;
}

const std::string &User::get_display_name()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->display_name;
}

const std::string &User::get_profile_pic()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->profile_pic;
}

const std::vector<UUID> &User::get_channels()
{
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->channels;
}

void User::set_username(std::string username)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->username = username;
}

void User::set_display_name(std::string display_name)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->display_name = display_name;
}

void User::set_profile_pic(std::string profile_pic)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->profile_pic = profile_pic;
}

void User::add_channel(UUID channel_id)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    this->channels.push_back(channel_id);
}

void User::remove_channel(UUID channel_id)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    std::remove_if(this->channels.begin(), this->channels.end(), [channel_id](UUID &c)
                   { return c == channel_id; });
}
