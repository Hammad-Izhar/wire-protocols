#include <gtest/gtest.h>
#include "server/db/database.hpp"
#include "models/uuid.hpp"
#include "models/user.hpp"
#include "models/channel.hpp"
#include "models/message.hpp"
#include <filesystem>

// Fixture for persistent Database tests.
class DatabasePersistentTestFixture : public ::testing::Test {
protected:
    std::string test_dir = "test_dir";

    void SetUp() override {
        if (std::filesystem::exists(test_dir))
            std::filesystem::remove_all(test_dir);
        std::filesystem::create_directory(test_dir);
    }

    void TearDown() override {
        if (std::filesystem::exists(test_dir))
            std::filesystem::remove_all(test_dir);
    }
};

TEST_F(DatabasePersistentTestFixture, SaveAndLoadAllRecords) {
    UUID userUid, channelUid;
    uint64_t messageSnowflake;

    {
        // Create a database instance and add user, channel, and message.
        Database db(test_dir);
        // Add a user.
        User::SharedPtr user = std::make_shared<User>("user1", "User One");
        auto userResult = db.add_user(user, "pass1");
        EXPECT_TRUE(std::holds_alternative<std::monostate>(userResult));
        userUid = user->get_uid();

        // Add a channel with the user as a member.
        std::vector<UUID> members = { userUid };
        auto channelResult = db.add_channel("ChannelOne", members);
        ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(channelResult));
        Channel::SharedPtr channel = std::get<Channel::SharedPtr>(channelResult);
        channelUid = channel->get_uid();

        // Add a message from the user to the channel.
        auto messageResult = db.add_message(userUid, channelUid, "Persistent message");
        ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(messageResult));
        Message::SharedPtr message = std::get<Message::SharedPtr>(messageResult);
        messageSnowflake = message->get_snowflake();
    }

    {
        // Reload a new Database instance from persistent storage.
        Database db(test_dir);
        // Verify the user.
        auto userOpt = db.get_user_by_uid(userUid);
        EXPECT_TRUE(userOpt.has_value());
        if(userOpt.has_value()){
            EXPECT_EQ(userOpt.value()->get_username(), "user1");
        }
        // Verify the channel.
        auto channelOpt = db.get_channel_by_uid(channelUid);
        EXPECT_TRUE(channelOpt.has_value());
        if(channelOpt.has_value()){
            EXPECT_EQ(channelOpt.value()->get_name(), "ChannelOne");
            // Verify that the channel has the user as a member.
            auto members = channelOpt.value()->get_user_uids();
            EXPECT_EQ(members.size(), 1);
            EXPECT_EQ(members[0].to_string(), userUid.to_string());
        }
        // Verify the message.
        auto messageOpt = db.get_message_by_uid(messageSnowflake);
        EXPECT_TRUE(messageOpt.has_value());
        if(messageOpt.has_value()){
            EXPECT_EQ(messageOpt.value()->get_text(), "Persistent message");
            EXPECT_EQ(messageOpt.value()->get_sender_id().to_string(), userUid.to_string());
            EXPECT_EQ(messageOpt.value()->get_channel_id().to_string(), channelUid.to_string());
        }
    }
}

TEST_F(DatabasePersistentTestFixture, RemoveRecordsPersistent) {
    UUID userUid, channelUid;
    uint64_t messageSnowflake;

    {
        // Create a database instance and add records.
        Database db(test_dir);
        User::SharedPtr user = std::make_shared<User>("user2", "User Two");
        db.add_user(user, "pass2");
        userUid = user->get_uid();

        std::vector<UUID> members = { userUid };
        auto channelResult = db.add_channel("ChannelTwo", members);
        ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(channelResult));
        Channel::SharedPtr channel = std::get<Channel::SharedPtr>(channelResult);
        channelUid = channel->get_uid();

        auto messageResult = db.add_message(userUid, channelUid, "Message to remove");
        ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(messageResult));
        Message::SharedPtr message = std::get<Message::SharedPtr>(messageResult);
        messageSnowflake = message->get_snowflake();

        // Remove the message and the user.
        auto removeMsgResult = db.remove_message(messageSnowflake);
        EXPECT_TRUE(std::holds_alternative<std::monostate>(removeMsgResult));
        auto removeUserResult = db.remove_user(userUid);
        EXPECT_TRUE(std::holds_alternative<User::SharedPtr>(removeUserResult));
    }

    {
        // Reload the database.
        Database db(test_dir);
        // Verify that the message is removed.
        auto msgOpt = db.get_message_by_uid(messageSnowflake);
        EXPECT_FALSE(msgOpt.has_value());
        // Verify that the user is removed.
        auto userOpt = db.get_user_by_uid(userUid);
        EXPECT_FALSE(userOpt.has_value());
        // Verify that the channel still exists.
        auto channelOpt = db.get_channel_by_uid(channelUid);
        EXPECT_TRUE(channelOpt.has_value());
    }
}

TEST_F(DatabasePersistentTestFixture, HammadsExactTest) {
    // Create a database instance and add records.

    uint64_t msg1_snowflake;
    uint64_t msg2_snowflake;

    UUID channel_uid;

    UUID alan_id;
    UUID ben_id;
    UUID charlie_id;
    {
        Database db(test_dir);
        User::SharedPtr alan = std::make_shared<User>("alan", "alan");
        db.add_user(alan, "alan");
        User::SharedPtr ben = std::make_shared<User>("ben", "ben");
        db.add_user(ben, "ben");
        User::SharedPtr charlie = std::make_shared<User>("charlie", "charlie");
        db.add_user(charlie, "charlie");

        alan_id = alan->get_uid();
        ben_id = ben->get_uid();
        charlie_id = charlie->get_uid();

        // Create a channel between alan and ben.
        std::vector<UUID> members = { alan->get_uid(), ben->get_uid() };
        auto channelResult = db.add_channel("alan, ben", members);
        ASSERT_TRUE(std::holds_alternative<Channel::SharedPtr>(channelResult));
        Channel::SharedPtr channel = std::get<Channel::SharedPtr>(channelResult);
        channel_uid = channel->get_uid();

        // Add a message from alan to the channel.
        auto messageResult = db.add_message(alan->get_uid(), channel->get_uid(), "alan to ben");

        ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(messageResult));
        Message::SharedPtr message1 = std::get<Message::SharedPtr>(messageResult);
        msg1_snowflake = message1->get_snowflake();

        // Add a message from ben to the channel.
        messageResult = db.add_message(ben->get_uid(), channel->get_uid(), "ben to alan");
        ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(messageResult));
        Message::SharedPtr message2 = std::get<Message::SharedPtr>(messageResult);
        msg2_snowflake = message2->get_snowflake();
    }

    // Reload the database
    Database dbReloaded(test_dir);

    // Verify that the channel exists.
    auto channelOpt = dbReloaded.get_channel_by_uid(channel_uid);
    EXPECT_TRUE(channelOpt.has_value());
    if(channelOpt.has_value()){
        // Verify that the channel has the correct members.
        auto members = channelOpt.value()->get_user_uids();
        EXPECT_EQ(members.size(), 2);
        EXPECT_EQ(members[0], alan_id);
        EXPECT_EQ(members[1], ben_id);
    }

    // Verify that the messages exist.
    auto messageOpt1 = dbReloaded.get_message_by_uid(msg1_snowflake);
    EXPECT_TRUE(messageOpt1.has_value());
    if(messageOpt1.has_value()){
        EXPECT_EQ(messageOpt1.value()->get_text(), "alan to ben");
        EXPECT_EQ(messageOpt1.value()->get_sender_id(), alan_id);
        EXPECT_EQ(messageOpt1.value()->get_channel_id(), channel_uid);
    }

    auto messageOpt2 = dbReloaded.get_message_by_uid(msg2_snowflake);
    EXPECT_TRUE(messageOpt2.has_value());
    if(messageOpt2.has_value()){
        EXPECT_EQ(messageOpt2.value()->get_text(), "ben to alan");
        EXPECT_EQ(messageOpt2.value()->get_sender_id(), ben_id);
        EXPECT_EQ(messageOpt2.value()->get_channel_id(), channel_uid);
    }


}
