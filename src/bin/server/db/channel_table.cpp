#include "server/db/channel_table.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <optional>
#include <variant>
#include <mutex>
#include <vector>

// Helper function to split a string by a given delimiter.
static std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

ChannelTable::ChannelTable(std::string db_dir_path) {
    // Use pipe '|' as the field delimiter.
    this->file_path = db_dir_path + "/channels.csv";

    // Check if the file exists; if not, create it.
    if (!std::filesystem::exists(this->file_path)) {
        std::ofstream file(this->file_path);
        if (file) {
            // Optionally, write a header:
            file << "uid|name|user_uids|message_snowflakes" << std::endl;
        } else {
            std::cerr << "Failed to create file: " << this->file_path << std::endl;
        }
    } else {
        // Read existing file and populate the in-memory map.
        std::ifstream file(this->file_path);
        if (file) {
            std::string line;
            // Read first line and check if it's a header.
            if (std::getline(file, line)) {
                bool isHeader = (line.find("uid") != std::string::npos);
                if (!isHeader) {
                    auto tokens = split(line, '|');
                    if (tokens.size() >= 3) {
                        // tokens: 0: uid, 1: name, 2: user_uids, 3: messages (optional)
                        UUID uid = UUID::from_string(tokens[0]);
                        std::string name = tokens[1];
                        std::vector<UUID> user_uids;
                        if (!tokens[2].empty()) {
                            auto userTokens = split(tokens[2], ';');
                            for (const auto& t : userTokens) {
                                user_uids.push_back(UUID::from_string(t));
                            }
                        }
                        std::vector<uint64_t> message_snowflakes;
                        std::string messages_field = (tokens.size() >= 4) ? tokens[3] : "";
                        if (!messages_field.empty()) {
                            auto snowflakeTokens = split(messages_field, ';');
                            for (const auto& s : snowflakeTokens) {
                                std::string trimmed = s;
                                trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
                                trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
                                try {
                                    uint64_t msg_snowflake = std::stoull(trimmed);
                                    message_snowflakes.push_back(msg_snowflake);
                                } catch (const std::exception& e) {
                                    std::cerr << "Failed to parse message snowflake: " 
                                              << trimmed << " - " << e.what() << std::endl;
                                }
                            }
                        }
                        auto channel = std::make_shared<Channel>(uid, name, user_uids);
                        for (const auto& ms : message_snowflakes) {
                            channel->add_message(ms);
                        }
                        this->data.insert({uid, channel});
                    }
                }
            }
            // Process remaining lines.
            while (std::getline(file, line)) {
                auto tokens = split(line, '|');
                if (tokens.size() >= 3) {
                    UUID uid = UUID::from_string(tokens[0]);
                    std::string name = tokens[1];
                    std::vector<UUID> user_uids;
                    if (!tokens[2].empty()) {
                        auto userTokens = split(tokens[2], ';');
                        for (const auto& t : userTokens) {
                            user_uids.push_back(UUID::from_string(t));
                        }
                    }
                    std::vector<uint64_t> message_snowflakes;
                    std::string messages_field = (tokens.size() >= 4) ? tokens[3] : "";
                    if (!messages_field.empty()) {
                        auto snowflakeTokens = split(messages_field, ';');
                        for (const auto& s : snowflakeTokens) {
                            std::string trimmed = s;
                            trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
                            trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
                            try {
                                uint64_t msg_snowflake = std::stoull(trimmed);
                                message_snowflakes.push_back(msg_snowflake);
                            } catch (const std::exception& e) {
                                std::cerr << "Failed to parse message snowflake: " 
                                          << trimmed << " - " << e.what() << std::endl;
                            }
                        }
                    }
                    auto channel = std::make_shared<Channel>(uid, name, user_uids);
                    for (const auto& ms : message_snowflakes) {
                        channel->add_message(ms);
                    }
                    this->data.insert({uid, channel});
                }
            }
            file.close();
        } else {
            std::cerr << "Failed to open file: " << this->file_path << std::endl;
        }
    }
}

std::optional<const Channel::SharedPtr> ChannelTable::get_by_uid(UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(channel_uid) != this->data.end()
               ? std::optional<const Channel::SharedPtr>(this->data.at(channel_uid))
               : std::nullopt;
}

std::optional<Channel::SharedPtr> ChannelTable::get_mut_by_uid(UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(channel_uid) != this->data.end()
               ? std::optional<Channel::SharedPtr>(this->data.at(channel_uid))
               : std::nullopt;
}

std::variant<Channel::SharedPtr, std::string> ChannelTable::add_channel(std::string channel_name, std::vector<UUID> members) {
    std::lock_guard<std::mutex> lock(this->mutex);
    Channel::SharedPtr channel = std::make_shared<Channel>(channel_name, members);
    this->data.insert({channel->get_uid(), channel});

    // Prepare CSV fields.
    std::string uid_str = channel->get_uid().to_string();
    std::string name = channel->get_name();

    // Convert vector of user UUIDs to a semicolon-separated list.
    std::string members_str;
    for (size_t i = 0; i < members.size(); ++i) {
        members_str += members[i].to_string();
        if (i != members.size() - 1) {
            members_str += ";";
        }
    }
    // For a new channel, there are no messages yet.
    std::string messages_str;

    // Append the new channel to the CSV file using '|' as the delimiter.
    std::ofstream file(this->file_path, std::ios::app);
    if (!file.is_open()) {
        return "Failed to open file for appending: " + this->file_path;
    }
    file << uid_str << "|" << name << "|" << members_str << "|" << messages_str << "\n";
    if (!file.good()) {
        return "Failed to write channel data to file: " + this->file_path;
    }
    return channel;
}

std::variant<std::monostate, std::string> ChannelTable::remove_channel(UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.erase(channel_uid);

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
    // Copy over all lines except the one that matches the channel UID.
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string first_token;
        if (std::getline(iss, first_token, '|')) {
            if (first_token == channel_uid.to_string()) {
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

std::variant<std::monostate, std::string> ChannelTable::add_message_to_channel(uint64_t message_snowflake, UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    
    // Look up the channel.
    auto it = this->data.find(channel_uid);
    if (it == this->data.end()) {
        return "Channel does not exist";
    }
    Channel::SharedPtr channel = it->second;
    
    // Add the message if not already present.
    bool exists = false;
    for (auto ms : channel->get_message_snowflakes()) {
        if (ms == message_snowflake) {
            exists = true;
            break;
        }
    }
    if (!exists) {
        channel->add_message(message_snowflake);
    }
    
    // Update the CSV file.
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
        // Split the line by '|'
        std::istringstream iss(line);
        std::string uid_field;
        if (std::getline(iss, uid_field, '|')) {
            if (uid_field == channel_uid.to_string()) {
                // Reconstruct the record using current channel data.
                // We assume the CSV format is: uid|name|user_uids|message_snowflakes
                // Read remaining fields (name and user_uids) from the line.
                std::string name, user_uids;
                std::getline(iss, name, '|');
                std::getline(iss, user_uids, '|');
                
                // Build a semicolon-separated list of message snowflakes.
                std::string messages_str;
                auto messages = channel->get_message_snowflakes();
                for (size_t i = 0; i < messages.size(); ++i) {
                    messages_str += std::to_string(messages[i]);
                    if (i != messages.size() - 1) {
                        messages_str += ";";
                    }
                }
                std::string newLine = uid_field + "|" + name + "|" + user_uids + "|" + messages_str;
                outfile << newLine << "\n";
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

std::variant<std::monostate, std::string> ChannelTable::remove_message_from_channel(uint64_t message_snowflake, UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);
    
    // Look up the channel.
    auto it = this->data.find(channel_uid);
    if (it == this->data.end()) {
        return "Channel does not exist";
    }
    Channel::SharedPtr channel = it->second;
    
    // Remove the message (if present).
    channel->remove_message(message_snowflake);
    
    // Update the CSV file.
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
        std::istringstream iss(line);
        std::string uid_field;
        if (std::getline(iss, uid_field, '|')) {
            if (uid_field == channel_uid.to_string()) {
                // Reconstruct the record.
                std::string name, user_uids;
                std::getline(iss, name, '|');
                std::getline(iss, user_uids, '|');
                
                std::string messages_str;
                auto messages = channel->get_message_snowflakes();
                for (size_t i = 0; i < messages.size(); ++i) {
                    messages_str += std::to_string(messages[i]);
                    if (i != messages.size() - 1) {
                        messages_str += ";";
                    }
                }
                std::string newLine = uid_field + "|" + name + "|" + user_uids + "|" + messages_str;
                outfile << newLine << "\n";
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

std::variant<std::monostate, std::string> ChannelTable::add_user_to_channel(UUID user_uid, UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);

    // Find the channel in the in-memory map.
    auto it = this->data.find(channel_uid);
    if (it == this->data.end()) {
        return "Channel does not exist";
    }
    Channel::SharedPtr channel = it->second;

    // Check if the user is already in the channel.
    bool alreadyPresent = false;
    for (const auto &uid : channel->get_user_uids()) {
        if (uid == user_uid) {
            alreadyPresent = true;
            break;
        }
    }
    if (!alreadyPresent) {
        channel->add_user(user_uid);
    }

    // Update the CSV file.
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
        // Tokenize by '|'
        std::istringstream iss(line);
        std::string uid_field;
        if (std::getline(iss, uid_field, '|')) {
            if (uid_field == channel_uid.to_string()) {
                // For this channel, we need to update the user_uids field.
                std::string name, members_field, messages_field;
                std::getline(iss, name, '|');
                std::getline(iss, members_field, '|'); // existing members (ignored)
                std::getline(iss, messages_field, '|'); // message_snowflakes field
                // Build updated members string.
                std::string updated_members;
                auto members = channel->get_user_uids();
                for (size_t i = 0; i < members.size(); ++i) {
                    updated_members += members[i].to_string();
                    if (i != members.size() - 1) {
                        updated_members += ";";
                    }
                }
                // Reconstruct the record.
                std::string newLine = uid_field + "|" + name + "|" + updated_members + "|" + messages_field;
                outfile << newLine << "\n";
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

std::variant<std::monostate, std::string> ChannelTable::remove_user_from_channel(UUID user_uid, UUID channel_uid) {
    std::lock_guard<std::mutex> lock(this->mutex);

    // Find the channel in the in-memory map.
    auto it = this->data.find(channel_uid);
    if (it == this->data.end()) {
        return "Channel does not exist";
    }
    Channel::SharedPtr channel = it->second;

    // Remove the user.
    channel->remove_user(user_uid);

    // Update the CSV file.
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
        std::istringstream iss(line);
        std::string uid_field;
        if (std::getline(iss, uid_field, '|')) {
            if (uid_field == channel_uid.to_string()) {
                // For this channel, update the user_uids field.
                std::string name, members_field, messages_field;
                std::getline(iss, name, '|');
                std::getline(iss, members_field, '|');
                std::getline(iss, messages_field, '|');
                // Build updated members string.
                std::string updated_members;
                auto members = channel->get_user_uids();
                for (size_t i = 0; i < members.size(); ++i) {
                    updated_members += members[i].to_string();
                    if (i != members.size() - 1) {
                        updated_members += ";";
                    }
                }
                std::string newLine = uid_field + "|" + name + "|" + updated_members + "|" + messages_field;
                outfile << newLine << "\n";
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

const std::unordered_map<UUID, Channel::SharedPtr>& ChannelTable::get_data() const {
    return this->data;
}

