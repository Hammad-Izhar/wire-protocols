#include <unordered_map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <optional>
#include <variant>
#include <mutex>
#include <vector>

#include "server/db/message_table.hpp"

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

MessageTable::MessageTable(std::string db_dir_path) {
    // Use pipe as the delimiter.
    this->file_path = db_dir_path + "/messages.csv";

    std::cout << "Trying to start reading messages from: " << this->file_path << std::endl;
    
    // Check if the file exists; if not, create it.
    if (!std::filesystem::exists(this->file_path)) {
        std::ofstream file(this->file_path);
        if (file) {
            std::cout << "Created file: " << this->file_path << std::endl;
            // Optionally, write a header:
            // file << "snowflake|sender_uid|channel_uid|created_at|modified_at|read_by|text" << std::endl;
        } else {
            std::cerr << "Failed to create file: " << this->file_path << std::endl;
        }
    } else {
        // If the file exists, read its contents into the in-memory data map.
        std::ifstream file(this->file_path);
        if (file) {
            std::cout << "Found the file! Reading message contents..." << std::endl;
            std::string line;
            // Check if the first line is a header (optional)
            if (std::getline(file, line)) {
                bool isHeader = (line.find("snowflake") != std::string::npos);
                if (!isHeader) {
                    // Process the first line as data.
                    auto tokens = split(line, '|');
                    if (tokens.size() >= 7) {
                        try {
                            uint64_t snowflake = std::stoull(tokens[0]);
                            UUID sender_uid = UUID::from_string(tokens[1]);
                            UUID channel_uid = UUID::from_string(tokens[2]);
                            uint64_t created_at = std::stoull(tokens[3]);
                            uint64_t modified_at = std::stoull(tokens[4]);
                            
                            // Parse the read_by field (semicolon-separated UUIDs).
                            std::vector<UUID> read_by;
                            if (!tokens[5].empty()) {
                                auto readTokens = split(tokens[5], ';');
                                for (const auto &t : readTokens) {
                                    std::string trimmed = t;
                                    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
                                    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
                                    if (!trimmed.empty())
                                        read_by.push_back(UUID::from_string(trimmed));
                                }
                            }
                            
                            std::string text = tokens[6];
                            auto message = std::make_shared<Message>(sender_uid, channel_uid, text, snowflake, created_at, modified_at, read_by);
                            this->data.insert({snowflake, message});
                        } catch (const std::exception& e) {
                            std::cerr << "Error parsing message record: " << e.what() << std::endl;
                        }
                    }
                }
            }
            // Process remaining lines.
            while (std::getline(file, line)) {
                auto tokens = split(line, '|');
                if (tokens.size() >= 7) {
                    try {
                        uint64_t snowflake = std::stoull(tokens[0]);
                        UUID sender_uid = UUID::from_string(tokens[1]);
                        UUID channel_uid = UUID::from_string(tokens[2]);
                        uint64_t created_at = std::stoull(tokens[3]);
                        uint64_t modified_at = std::stoull(tokens[4]);
                        
                        std::vector<UUID> read_by;
                        if (!tokens[5].empty()) {
                            auto readTokens = split(tokens[5], ';');
                            for (const auto &t : readTokens) {
                                std::string trimmed = t;
                                trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
                                trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
                                if (!trimmed.empty())
                                    read_by.push_back(UUID::from_string(trimmed));
                            }
                        }
                        
                        std::string text = tokens[6];
                        auto message = std::make_shared<Message>(sender_uid, channel_uid, text, snowflake, created_at, modified_at, read_by);
                        this->data.insert({snowflake, message});
                    } catch (const std::exception& e) {
                        std::cerr << "Error parsing message record: " << e.what() << std::endl;
                    }
                }
            }
            file.close();
        } else {
            std::cerr << "Failed to open file: " << this->file_path << std::endl;
        }
    }
    std::cout << "Finished reading messages" << std::endl;
}

std::optional<const Message::SharedPtr> MessageTable::get_by_uid(uint64_t message_snowflake) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(message_snowflake) != this->data.end()
               ? std::optional<const Message::SharedPtr>(this->data.at(message_snowflake))
               : std::nullopt;
}

std::optional<Message::SharedPtr> MessageTable::get_mut_by_uid(uint64_t message_snowflake) {
    std::lock_guard<std::mutex> lock(this->mutex);
    return this->data.find(message_snowflake) != this->data.end()
               ? std::optional<Message::SharedPtr>(this->data.at(message_snowflake))
               : std::nullopt;
}

std::variant<Message::SharedPtr, std::string> MessageTable::add_message(UUID sender_uid,
                                                                        UUID channel_uid,
                                                                        std::string content) {
    std::lock_guard<std::mutex> lock(this->mutex);

    // Create a new message.
    auto message = std::make_shared<Message>(sender_uid, channel_uid, content);
    this->data.insert({message->get_snowflake(), message});

    // Open the CSV file in append mode.
    std::ofstream file(this->file_path, std::ios::app);
    if (!file.is_open()) {
        return "Failed to open file for appending: " + this->file_path;
    }

    // Prepare the read_by field as a semicolon-separated list.
    // When a message is first created, no user has read it.
    std::string read_by_str = "";

    // Write the new message's data as a CSV line using '|' as the delimiter.
    file << message->get_snowflake() << "|"
         << message->get_sender_id().to_string() << "|"
         << message->get_channel_id().to_string() << "|"
         << message->get_created_at() << "|"
         << message->get_modified_at() << "|"
         << read_by_str << "|"
         << message->get_text() << "\n";

    if (!file.good()) {
        return "Failed to write message data to file: " + this->file_path;
    }

    return message;
}

std::variant<std::monostate, std::string> MessageTable::remove_message(uint64_t message_snowflake) {
    std::lock_guard<std::mutex> lock(this->mutex);
    this->data.erase(message_snowflake);

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
    // Copy over all lines except the one that matches the message's snowflake.
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string first_token;
        if (std::getline(iss, first_token, '|')) {
            if (first_token == std::to_string(message_snowflake)) {
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
