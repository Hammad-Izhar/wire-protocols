#include "server/db/user_table.hpp"
#include <regex>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <memory>
#include <vector>

// Helper function to split a string by a delimiter.
static std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

UserTable::UserTable(std::string db_dir_path) {
    // Use pipe as the delimiter.
    this->file_path = db_dir_path + "/users.csv";

    // Check if the file exists; if not, create it.
    if (!std::filesystem::exists(this->file_path)) {
        std::ofstream file(this->file_path);
        if (file) {
            // Optionally, write a header:
            file << "uid|username|display_name|profile_pic|channels" << std::endl;
        } else {
            std::cerr << "Failed to create file: " << this->file_path << std::endl;
        }
    } else {
        std::cout << "File found!" << std::endl;
        // If the file exists, read its contents into the in-memory map.
        std::ifstream file(this->file_path);
        if (file) {
            std::string line;
            // Check if the first line is a header (optional)
            if (std::getline(file, line)) {
                bool isHeader = (line.find("uid") != std::string::npos);
                if (!isHeader) {
                    std::cout << "THIS SHOULD NOT HAPPEN" << std::endl;
                    auto tokens = split(line, '|');
                    if (tokens.size() >= 5) {
                        UUID uid = UUID::from_string(tokens[0]);
                        std::string username = tokens[1];
                        std::string display_name = tokens[2];
                        std::string profile_pic = tokens[3];
                        std::vector<UUID> channels;
                        if (!tokens[4].empty()) {
                            auto channelTokens = split(tokens[4], ';');
                            for (const auto& t : channelTokens) {
                                channels.push_back(UUID::from_string(t));
                            }
                        }
                        auto user = std::make_shared<User>(username, display_name, uid, profile_pic);
                        for (const auto& ch : channels) {
                            user->add_channel(ch);
                        }
                        this->data.insert({uid, user});
                    }
                }
            }
            // Process the remaining lines.
            while (std::getline(file, line)) {
                std::cout << "LINE: " << line << std::endl;
                auto tokens = split(line, '|');
                if (tokens.size() >= 4) {
                    std::cout << "     Adding user" << std::endl;
                    UUID uid = UUID::from_string(tokens[0]);
                    std::string username = tokens[1];
                    std::string display_name = tokens[2];
                    std::string profile_pic = tokens[3];
                    std::vector<UUID> channels;
                    // If tokens[4] doesn't exist, it means the user has no channels. Skip this step.
                    if (tokens.size() >= 5) {
                        if (!tokens[4].empty()) {
                            auto channelTokens = split(tokens[4], ';');
                            for (const auto& t : channelTokens) {
                                channels.push_back(UUID::from_string(t));
                            }
                        }
                    }
            
                    auto user = std::make_shared<User>(username, display_name, uid, profile_pic);
                    for (const auto& ch : channels) {
                        user->add_channel(ch);
                    }
                    this->data.insert({uid, user});
                } else {
                    std::cout << "     Not Adding. " << line << std::endl;
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

std::variant<std::monostate, std::string> UserTable::add_user(User::SharedPtr user) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.insert({user->get_uid(), user});

    // Open the CSV file in append mode.
    std::ofstream file(this->file_path, std::ios::app);
    if (!file.is_open()) {
        return "Failed to open file for appending: " + this->file_path;
    }

    // Prepare channels field as a semicolon-separated list.
    std::string channels_str;
    auto channels = user->get_channels();
    for (size_t i = 0; i < channels.size(); ++i) {
        channels_str += channels[i].to_string();
        if (i != channels.size() - 1) {
            channels_str += ";";
        }
    }

    // Write the new user's information as a CSV line using '|' as delimiter.
    file << user->get_uid().to_string() << "|"
         << user->get_username() << "|"
         << user->get_display_name() << "|"
         << user->get_profile_pic() << "|"
         << channels_str << "\n";

    if (!file.good()) {
        return "Failed to write user data to file: " + this->file_path;
    }

    return {};
}

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
        if (std::getline(iss, first_token, '|')) {
            // If this line belongs to the user we're removing, skip it.
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

    return user;
}

std::variant<std::monostate, std::string> UserTable::add_channel_to_user(UUID user_uid, UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    // Find the user in the in-memory map.
    auto it = this->data.find(user_uid);
    if (it == this->data.end()) {
        return "User does not exist";
    }
    User::SharedPtr user = it->second;
    // Add the channel if not already present.
    bool alreadyExists = false;
    for (const auto &ch : user->get_channels()) {
        if (ch == channel_uid) {
            alreadyExists = true;
            break;
        }
    }
    if (!alreadyExists) {
        user->add_channel(channel_uid);
    }
    
    // Now update the CSV file.
    std::ifstream infile(this->file_path);
    if (!infile.is_open()) {
        return "Failed to open file for reading: " + this->file_path;
    }
    std::string temp_file_path = this->file_path + ".tmp";
    std::ofstream outfile(temp_file_path);
    if (!outfile.is_open()) {
        return "Failed to open temporary file for writing: " + temp_file_path;
    }
    
    std::string line;
    while (std::getline(infile, line)) {
        // Expecting format: uid|username|display_name|profile_pic|channels
        auto tokens = split(line, '|');
        if (tokens.size() < 5) {
            outfile << line << "\n";
            continue;
        }
        if (tokens[0] == user_uid.to_string()) {
            // Build updated channels string.
            std::string channels_str;
            auto updatedChannels = user->get_channels();
            for (size_t i = 0; i < updatedChannels.size(); ++i) {
                channels_str += updatedChannels[i].to_string();
                if (i < updatedChannels.size() - 1) {
                    channels_str += ";";
                }
            }
            // Reconstruct the line with updated channels.
            std::string newLine = tokens[0] + "|" + tokens[1] + "|" + tokens[2] + "|" + tokens[3] + "|" + channels_str;
            outfile << newLine << "\n";
        } else {
            outfile << line << "\n";
        }
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

std::variant<std::monostate, std::string> UserTable::remove_channel_from_user(UUID user_uid, UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    // Find the user in the in-memory map.
    auto it = this->data.find(user_uid);
    if (it == this->data.end()) {
        return "User does not exist";
    }
    User::SharedPtr user = it->second;
    // Remove the channel if it exists.
    user->remove_channel(channel_uid);
    
    // Now update the CSV file.
    std::ifstream infile(this->file_path);
    if (!infile.is_open()) {
        return "Failed to open file for reading: " + this->file_path;
    }
    std::string temp_file_path = this->file_path + ".tmp";
    std::ofstream outfile(temp_file_path);
    if (!outfile.is_open()) {
        return "Failed to open temporary file for writing: " + temp_file_path;
    }
    
    std::string line;
    while (std::getline(infile, line)) {
        auto tokens = split(line, '|');
        if (tokens.size() < 5) {
            outfile << line << "\n";
            continue;
        }
        if (tokens[0] == user_uid.to_string()) {
            // Build updated channels string.
            std::string channels_str;
            auto updatedChannels = user->get_channels();
            for (size_t i = 0; i < updatedChannels.size(); ++i) {
                channels_str += updatedChannels[i].to_string();
                if (i < updatedChannels.size() - 1) {
                    channels_str += ";";
                }
            }
            // Reconstruct the line with updated channels.
            std::string newLine = tokens[0] + "|" + tokens[1] + "|" + tokens[2] + "|" + tokens[3] + "|" + channels_str;
            outfile << newLine << "\n";
        } else {
            outfile << line << "\n";
        }
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

const std::unordered_map<UUID, User::SharedPtr>& UserTable::get_data() const {
    return this->data;
}
