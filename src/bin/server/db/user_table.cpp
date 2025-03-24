#include "server/db/user_table.hpp"
#include <regex>

#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <vector>

// Assuming necessary includes for User and UUID

UserTable::UserTable(std::string db_dir_path) {
    this->file_path = db_dir_path + "/users.csv";

    // Check if the file exists; if not, create it.
    if (!std::filesystem::exists(this->file_path)) {
        std::ofstream file(this->file_path);
        if (file) {
            std::cout << "Created file: " << this->file_path << std::endl;
            // Optionally, write a header to the CSV file:
            // file << "uid,username,display_name,profile_pic" << std::endl;
        } else {
            std::cerr << "Failed to create file: " << this->file_path << std::endl;
        }
    } else {
        // If the file exists, read the contents into the unordered_map.
        std::ifstream file(this->file_path);
        if (file) {
            std::cout << "Found the file! Reading contents..." << std::endl;
            std::string line;
            // Check if the first line is a header (optional)
            if (std::getline(file, line)) {
                bool isHeader = (line.find("uid") != std::string::npos);
                if (!isHeader) {
                    std::cout << "First line is not a header; processing as data." << std::endl;

                    // If not a header, process the first line as data.
                    std::istringstream iss(line);
                    std::string token;
                    std::vector<std::string> tokens;
                    while (std::getline(iss, token, ',')) {
                        tokens.push_back(token);
                    }
                    if (tokens.size() >= 4) {
                        UUID uid = UUID::from_string(tokens[0]);
                        auto user = std::make_shared<User>(tokens[1], tokens[2], uid, tokens[3]);
                        this->data.insert({uid, user});
                    }
                } else {
                    std::cout << "Skipping header line: " << line << std::endl;
                }
            }
            // Process the remaining lines.
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string token;
                std::vector<std::string> tokens;
                while (std::getline(iss, token, ',')) {
                    tokens.push_back(token);
                }
                
                std::cout << "Processing line: " << line << std::endl;

                if (tokens.size() >= 4) {
                    UUID uid = UUID::from_string(tokens[0]);
                    auto user = std::make_shared<User>(tokens[1], tokens[2], uid, tokens[3]);
                    this->data.insert({uid, user});
                }
            }
            file.close();
        } else {
            std::cerr << "Failed to open file: " << this->file_path << std::endl;
        }
    }
}


std::optional<const User::SharedPtr> UserTable::get_by_uid(UUID user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(user_uid) != this->data.end()
               ? std::optional<const User::SharedPtr>(this->data.at(user_uid))
               : std::nullopt;
}

std::optional<User::SharedPtr> UserTable::get_mut_by_uid(UUID user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(user_uid) != this->data.end()
               ? std::optional<User::SharedPtr>(this->data.at(user_uid))
               : std::nullopt;
}

std::variant<std::vector<UUID>, std::string> UserTable::get_uuids_matching_regex(std::string regex) {
    std::lock_guard<std::mutex> lock(this->mutex);
    std::vector<UUID> uuids;
    std::regex re;

    try {
        re = std::regex(regex);
    } catch (const std::regex_error& e) {
        return std::string("Regex error: ") + e.what();
    }

    for (const auto& [uid, user] : this->data) {
        if (std::regex_match(user->get_username(), re)) {
            uuids.push_back(uid);
        }
    }
    return uuids;
}

std::optional<UUID> UserTable::get_uid_from_username(std::string username) {
    std::lock_guard<std::mutex> lock(this->mutex);
    for (const auto& [uid, user] : this->data) {
        if (user->get_username() == username) {
            return uid;
        }
    }
    return std::nullopt;
}

#include <fstream>

std::variant<std::monostate, std::string> UserTable::add_user(User::SharedPtr user) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.insert({user->get_uid(), user});

    // Open the CSV file in append mode.
    std::ofstream file(this->file_path, std::ios::app);
    if (!file.is_open()) {
        return "Failed to open file for appending: " + this->file_path;
    }

    // Write the new user's information as a CSV line.
    file << user->get_uid().to_string() << ","
         << user->get_username() << ","
         << user->get_display_name() << ","
         << user->get_profile_pic() << "\n";

    if (!file.good()) {
        return "Failed to write user data to file: " + this->file_path;
    }

    return {};
}

#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>

std::variant<User::SharedPtr, std::string> UserTable::remove_user(UUID user_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    auto it = this->data.find(user_uid);
    if (it == this->data.end()) {
        return "User does not exist";
    }
    
    User::SharedPtr user = it->second;
    this->data.erase(it);

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
    // Read each line and write it to the temporary file if it doesn't match the user's UID.
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string first_token;
        if (std::getline(iss, first_token, ',')) {
            // If this line belongs to the user we're removing, skip it.
            if (first_token == user_uid.to_string()) {
                continue;
            }
        }
        outfile << line << "\n";
    }

    infile.close();
    outfile.close();

    // Replace the original file with the temporary file.
    std::error_code ec;
    std::filesystem::remove(this->file_path, ec);
    if (ec) {
        return "Failed to remove original file: " + this->file_path;
    }
    std::filesystem::rename(temp_file_path, this->file_path, ec);
    if (ec) {
        return "Failed to rename temporary file: " + ec.message();
    }

    return user;
}
