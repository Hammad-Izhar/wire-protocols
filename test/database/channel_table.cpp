#include <gtest/gtest.h>
#include "server/db/channel_table.hpp"
#include "models/uuid.hpp"
#include "models/channel.hpp"
TEST(ChannelTableTest, AddChannelSuccessfully) {
    ChannelTable channelTable;
    UUID user1;
    UUID user2;
    std::vector<UUID> members = {user1, user2};

    auto result = channelTable.add_channel("General", members);
    EXPECT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
}

TEST(ChannelTableTest, GetChannelByUid) {
    ChannelTable channelTable;
    UUID user1;
    UUID user2;
    std::vector<UUID> members = {user1, user2};

    auto result = channelTable.add_channel("General", members);
    ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
    UUID channelUid = std::get<Channel::SharedPtr>(result)->get_uid();
    auto retrievedChannel = channelTable.get_by_uid(channelUid);
    EXPECT_TRUE(retrievedChannel.has_value());
}

TEST(ChannelTableTest, GetMutChannelByUid) {
    ChannelTable channelTable;
    UUID user1;
    UUID user2;
    std::vector<UUID> members = {user1, user2};
    auto result = channelTable.add_channel("General", members);
    ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
    UUID channelUid = std::get<Channel::SharedPtr>(result)->get_uid();
    auto retrievedChannel = channelTable.get_mut_by_uid(channelUid);
    EXPECT_TRUE(retrievedChannel.has_value());
}

TEST(ChannelTableTest, RemoveChannelSuccessfully) {
    ChannelTable channelTable;
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

TEST(ChannelTableTest, RemoveNonexistentChannel) {
    ChannelTable channelTable;
    UUID channelUid;
    auto result = channelTable.remove_channel(channelUid);
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

TEST(ChannelTableTest, GetNonexistentChannel) {
    ChannelTable channelTable;
    UUID channelUid;
    auto retrievedChannel = channelTable.get_by_uid(channelUid);
    EXPECT_FALSE(retrievedChannel.has_value());
}
