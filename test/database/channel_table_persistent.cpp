#include <gtest/gtest.h>
#include "server/db/channel_table.hpp"
#include "models/uuid.hpp"
#include "models/channel.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unordered_map>
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

// Test fixture that creates a temporary directory ("test_dir") for persistent storage.
class ChannelTableTestFixture : public ::testing::Test {
protected:
    std::string test_dir = "test_dir";

    virtual void SetUp() override {
        // Remove any existing test directory and create a fresh one.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
        std::filesystem::create_directory(test_dir);
    }

    virtual void TearDown() override {
        // Clean up test directory.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(ChannelTableTestFixture, AddChannelSuccessfully) {
    ChannelTable channelTable(test_dir);
    UUID user1;
    UUID user2;
    std::vector<UUID> members = {user1, user2};

    auto result = channelTable.add_channel("General", members);
    EXPECT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
}

TEST_F(ChannelTableTestFixture, GetChannelByUid) {
    ChannelTable channelTable(test_dir);
    UUID user1;
    UUID user2;
    std::vector<UUID> members = {user1, user2};

    auto result = channelTable.add_channel("General", members);
    ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
    UUID channelUid = std::get<Channel::SharedPtr>(result)->get_uid();
    auto retrievedChannel = channelTable.get_by_uid(channelUid);
    EXPECT_TRUE(retrievedChannel.has_value());
}

TEST_F(ChannelTableTestFixture, GetMutChannelByUid) {
    ChannelTable channelTable(test_dir);
    UUID user1;
    UUID user2;
    std::vector<UUID> members = {user1, user2};

    auto result = channelTable.add_channel("General", members);
    ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
    UUID channelUid = std::get<Channel::SharedPtr>(result)->get_uid();
    auto retrievedChannel = channelTable.get_mut_by_uid(channelUid);
    EXPECT_TRUE(retrievedChannel.has_value());
}

TEST_F(ChannelTableTestFixture, RemoveChannelSuccessfully) {
    ChannelTable channelTable(test_dir);
    UUID user1;
    UUID user2;
    std::vector<UUID> members = {user1, user2};

    auto result = channelTable.add_channel("General", members);
    ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
    UUID channelUid = std::get<Channel::SharedPtr>(result)->get_uid();
    auto removeResult = channelTable.remove_channel(channelUid);
    EXPECT_TRUE(std::holds_alternative<std::monostate>(removeResult));
    auto retrievedChannel = channelTable.get_by_uid(channelUid);
    EXPECT_FALSE(retrievedChannel.has_value());
}

TEST_F(ChannelTableTestFixture, RemoveNonexistentChannel) {
    ChannelTable channelTable(test_dir);
    UUID channelUid; // Default constructed UID (assumed nonexistent)
    auto result = channelTable.remove_channel(channelUid);
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

TEST_F(ChannelTableTestFixture, GetNonexistentChannel) {
    ChannelTable channelTable(test_dir);
    UUID channelUid; // Default constructed UID
    auto retrievedChannel = channelTable.get_by_uid(channelUid);
    EXPECT_FALSE(retrievedChannel.has_value());
}

TEST_F(ChannelTableTestFixture, PersistentStorageConsistency) {
    // Create a ChannelTable and add a channel.
    {
        ChannelTable channelTable(test_dir);
        UUID user1;
        UUID user2;
        std::vector<UUID> members = {user1, user2};
        auto result = channelTable.add_channel("General", members);
        ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
    }
    // Now reload a new ChannelTable from persistent storage.
    {
        ChannelTable reloadedTable(test_dir);
        const auto& data = reloadedTable.get_data(); // get_data() returns the internal map.
        // Expect exactly one channel.
        EXPECT_EQ(data.size(), 1);
        auto it = data.begin();
        Channel::SharedPtr channel = it->second;
        EXPECT_EQ(channel->get_name(), "General");
        auto members = channel->get_user_uids();
        EXPECT_EQ(members.size(), 2);

        // Additionally, verify the CSV file contents.
        std::ifstream file(test_dir + "/channels.csv");
        ASSERT_TRUE(file.is_open());
        std::string line;
        std::cout << "Reading CSV file contents:\n";
        // Skip header.
        std::getline(file, line);
        std::cout << line << std::endl;
        // Read the data line.
        ASSERT_TRUE(std::getline(file, line));
        std::cout << line << std::endl;
        auto tokens = split(line, '|');
        // Expect 4 tokens: uid, name, user_uids, message_snowflakes.
        EXPECT_EQ(tokens.size(), 3);
        EXPECT_EQ(tokens[1], "General");
        // The user_uids token should have 2 members separated by ';'.
        auto memberTokens = split(tokens[2], ';');
        EXPECT_EQ(memberTokens.size(), 2);
    }
}