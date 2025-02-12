#include <random>
#include "models/user.hpp"
#include <stdexcept>


User::User(std::string username, std::string password, std::string display_name)
    : username(username), password(password), display_name(display_name) 
{
    // Generate a random UID
    this->uid = UUID();

    // Set profile pic to default (Grey question mark)
    this->profile_pic = "./assets/profile_pics/blank_profile_pic.png";

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

void User::add_channel(UUID channel)
{
    this->channels.push_back(channel);
}

void User::remove_channel(UUID channel)
{
    auto it = std::find(this->channels.begin(), this->channels.end(), channel);
    if (it != this->channels.end()) {
        this->channels.erase(it);
    } else {
        throw std::runtime_error("Channel not found: " + channel.toString());
    }
}
