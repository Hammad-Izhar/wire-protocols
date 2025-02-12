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

const std::string &User::get_username() const
{
    return this->username;
}

const UUID &User::get_uid() const
{
    return this->uid;
}

const std::string &User::get_display_name() const
{
    return this->display_name;
}

const std::string &User::get_profile_pic() const
{
    return this->profile_pic;
}

const std::vector<UUID> &User::get_channels() const
{
    return this->channels;
}

void User::set_username(std::string username)
{
    this->username = username;
}

void User::set_display_name(std::string display_name)
{
    this->display_name = display_name;
}

void User::set_profile_pic(std::string profile_pic)
{
    this->profile_pic = profile_pic;
}

void User::add_channel(UUID channel_id)
{
    this->channels.push_back(channel_id);
}

void User::remove_channel(UUID channel_id)
{
    std::remove_if(this->channels.begin(), this->channels.end(), [channel_id](UUID &c)
                   { return c == channel_id; });
}
