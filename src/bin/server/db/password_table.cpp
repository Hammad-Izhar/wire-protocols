#include <openssl/evp.h>
#include <random>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <mutex>

#include "server/db/password_table.hpp"

PasswordTable::PasswordTable(std::string db_dir_path) {
    this->file_path = db_dir_path + "/passwords.csv";

    // Check if the file exists; if not, create it.
    if (!std::filesystem::exists(this->file_path)) {
        std::ofstream file(this->file_path);
        if (file) {
            // Optionally, write a header to the CSV file:
            file << "uid|hashed_password|salt" << std::endl;
        } else {
            std::cerr << "Failed to create file: " << this->file_path << std::endl;
        }
    } else {
        // If the file exists, read the contents into the unordered_map.
        std::ifstream file(this->file_path);
        if (file) {
            std::string line;
            // Check if the first line is a header (optional)
            if (std::getline(file, line)) {
                bool isHeader = (line.find("uid") != std::string::npos);
                if (!isHeader) {
                    std::istringstream iss(line);
                    std::string token;
                    std::vector<std::string> tokens;
                    while (std::getline(iss, token, '|')) {
                        tokens.push_back(token);
                    }
                    // Expecting: uid,hashed_password,salt
                    if (tokens.size() >= 3) {
                        UUID uid = UUID::from_string(tokens[0]);
                        this->data.insert({uid, std::make_pair(tokens[1], tokens[2])});
                    }
                }
            }
            // Process the remaining lines.
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(iss, token, '|')) {
                    tokens.push_back(token);
                }
            

                if (tokens.size() >= 3) {
                    UUID uid = UUID::from_string(tokens[0]);
                    this->data.insert({uid, std::make_pair(tokens[1], tokens[2])});
                }
            }
            file.close();
        } else {
            std::cerr << "Failed to open file: " << this->file_path << std::endl;
        }
    }
}

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
    // A simple hash implementation (DJB2 algorithm)
    unsigned long hash = 5381;
    for (unsigned char c : str) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    std::ostringstream oss;
    oss << std::hex << hash;
    return oss.str();
}

std::variant<std::monostate, std::string> PasswordTable::add_password(UUID& user_uid,
                                                                      std::string password) {
    std::lock_guard<std::mutex> lock(this->mutex);
    std::string salt = generate_salt();
    std::string hashed = sha256(password + salt);
    this->data.insert({user_uid, std::make_pair(hashed, salt)});

    // Append the new password entry to the CSV file.
    std::ofstream file(this->file_path, std::ios::app);
    if (!file.is_open()) {
        return "Failed to open file for appending: " + this->file_path;
    }
    file << user_uid.to_string() << "," << hashed << "," << salt << "\n";
    if (!file.good()) {
        return "Failed to write password data to file: " + this->file_path;
    }
    return {};
}

std::variant<std::monostate, std::string> PasswordTable::remove_password(UUID& user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    if (this->data.find(user_uid) == this->data.end()) {
        return "User does not exist";
    }
    this->data.erase(user_uid);

    // Open the CSV file for reading.
    std::ifstream infile(this->file_path);
    if (!infile.is_open()) {
        return "Failed to open file for reading: " + this->file_path;
    }

    // Create a temporary file.
    std::string temp_file_path = this->file_path + ".tmp";
    std::ofstream outfile(temp_file_path);
    if (!outfile.is_open()) {
        return "Failed to open temporary file for writing: " + temp_file_path;
    }

    std::string line;
    // Copy over all lines except the one matching the user's UID.
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string first_token;
        if (std::getline(iss, first_token, ',')) {
            if (first_token == user_uid.to_string()) {
                continue;
            }
        }
        outfile << line << "\n";
    }

    infile.close();
    outfile.close();

    std::error_code ec;
    std::filesystem::remove(this->file_path, ec);
    if (ec) {
        return "Failed to remove original file: " + this->file_path;
    }
    std::filesystem::rename(temp_file_path, this->file_path, ec);
    if (ec) {
        return "Failed to rename temporary file: " + ec.message();
    }

    return {};
}
