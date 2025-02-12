#include <openssl/evp.h>
#include <iomanip>
#include <random>
#include <sstream>

#include "server/db/password_table.hpp"

std::variant<bool, std::string> PasswordTable::verify_password(UUID& user_uid,
                                                               std::string password) {
    std::lock_guard<std::mutex> lock(this->mutex);
    if (this->data.find(user_uid) == this->data.end()) {
        return "User does not exist";
    }
    std::pair<std::string, std::string>& user_data = this->data.at(user_uid);
    if (user_data.first != sha256(password + user_data.second)) {
        return false;
    }
    return true;
}

std::string PasswordTable::generate_salt() {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    std::string salt;
    for (size_t i = 0; i < 16; i++) {
        salt += charset[dist(generator)];
    }
    return salt;
}

std::string PasswordTable::sha256(const std::string& str) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    // Initialize the context with SHA-256
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize SHA-256");
    }

    // Update the context with input data
    if (EVP_DigestUpdate(ctx, str.data(), str.size()) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to update digest");
    }

    // Finalize and get the hash result
    std::vector<unsigned char> hash(EVP_MD_size(EVP_sha256()));
    unsigned int length = 0;
    if (EVP_DigestFinal_ex(ctx, hash.data(), &length) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize digest");
    }

    // Cleanup
    EVP_MD_CTX_free(ctx);

    // Convert hash to a hex string
    std::ostringstream hex_stream;
    for (unsigned char byte : hash) {
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return hex_stream.str();
}

std::variant<std::monostate, std::string> PasswordTable::add_password(UUID& user_uid,
                                                                      std::string password) {
    std::lock_guard<std::mutex> lock(this->mutex);
    std::string salt = generate_salt();
    this->data.insert({user_uid, std::make_pair(sha256(password + salt), salt)});

    return {};
}

std::variant<std::monostate, std::string> PasswordTable::remove_password(UUID& user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    if (this->data.find(user_uid) == this->data.end()) {
        return "User does not exist";
    }
    this->data.erase(user_uid);

    return {};
}