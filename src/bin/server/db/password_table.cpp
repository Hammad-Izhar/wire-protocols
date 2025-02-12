#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <random>

#include "server/db/password_table.hpp"

std::variant<bool, std::string> PasswordTable::verify_password(UUID &user_uid, std::string password)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (this->data.find(user_uid) == this->data.end())
    {
        return "User does not exist";
    }
    std::pair<std::string, std::string> &user_data = this->data.at(user_uid);
    if (user_data.first != sha256(password + user_data.second))
    {
        return false;
    }
    return true;
}

std::string PasswordTable::generate_salt()
{
    static const char charset[] =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string salt;
    for (size_t i = 0; i < 16; i++)
    {
        salt += charset[dist(generator)];
    }
    return salt;
}

std::string PasswordTable::sha256(const std::string &str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.size());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (unsigned char i : hash)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)i;
    }
    return ss.str();
}

std::variant<std::monostate, std::string> PasswordTable::add_password(UUID &user_uid, std::string password)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    std::string salt = generate_salt();
    this->data.insert({user_uid, std::make_pair(sha256(password + salt), salt)});

    return {};
}

std::variant<std::monostate, std::string> PasswordTable::remove_password(UUID &user_uid)
{
    std::lock_guard<std::mutex> lock(this->mutex);
    if (this->data.find(user_uid) == this->data.end())
    {
        return "User does not exist";
    }
    this->data.erase(user_uid);

    return {};
}