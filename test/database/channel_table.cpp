#include <gtest/gtest.h>
#include "server/db/channel_table.hpp"
#include "models/uuid.hpp"
#include "models/channel.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

// Test fixture for ChannelTable using "test_dir" as the database directory.
class ChannelTableTestFixture : public ::testing::Test {
protected:
    std::string test_dir = "test_dir";
    
    void SetUp() override {
        // Remove any existing test directory and create a fresh one.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
        std::filesystem::create_directory(test_dir);
    }
    
    void TearDown() override {
        // Clean up the test directory.
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

    // Reload a new ChannelTable from persistent storage and verify consistency.
    ChannelTable reloadedTable(test_dir);
    const auto& data = reloadedTable.get_data();
    EXPECT_EQ(data.size(), 1);
    auto it = data.begin();
    Channel::SharedPtr channel = it->second;
    EXPECT_EQ(channel->get_name(), "General");
    auto reloadedMembers = channel->get_user_uids();
    EXPECT_EQ(reloadedMembers.size(), 2);
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
    UUID channelUid; // Default-constructed (assumed nonexistent)
    auto result = channelTable.remove_channel(channelUid);
    // The implementation returns monostate when nothing is removed.
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

TEST_F(ChannelTableTestFixture, GetNonexistentChannel) {
    ChannelTable channelTable(test_dir);
    UUID channelUid; // Default-constructed
    auto retrievedChannel = channelTable.get_by_uid(channelUid);
    EXPECT_FALSE(retrievedChannel.has_value());
}
