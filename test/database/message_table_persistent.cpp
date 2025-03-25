#include <gtest/gtest.h>
#include "server/db/message_table.hpp"
#include "models/message.hpp"
#include "models/uuid.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

// Fixture for persistent MessageTable tests.
class MessageTablePersistentTestFixture : public ::testing::Test {
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
        // Remove the test directory.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(MessageTablePersistentTestFixture, SaveAndLoadMessage) {
    UUID sender;
    UUID channel;
    uint64_t snowflake;
    {
        // Create a MessageTable and add a message.
        MessageTable table(test_dir);
        auto result = table.add_message(sender, channel, "Persistent message test");
        ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(result));
        auto msg = std::get<Message::SharedPtr>(result);
        snowflake = msg->get_snowflake();
    }
    {
        // Reload a new MessageTable from persistent storage.
        MessageTable reloadedTable(test_dir);
        auto msg_opt = reloadedTable.get_by_uid(snowflake);
        EXPECT_TRUE(msg_opt.has_value());
        if (msg_opt.has_value()) {
            Message::SharedPtr msg = msg_opt.value();
            EXPECT_EQ(msg->get_text(), "Persistent message test");
            EXPECT_EQ(msg->get_sender_id().to_string(), sender.to_string());
            EXPECT_EQ(msg->get_channel_id().to_string(), channel.to_string());
        }
    }
}

TEST_F(MessageTablePersistentTestFixture, RemoveMessagePersistent) {
    UUID sender;
    UUID channel;
    uint64_t snowflake;
    {
        // Create a MessageTable, add a message, then remove it.
        MessageTable table(test_dir);
        auto result = table.add_message(sender, channel, "Message to remove");
        ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(result));
        auto msg = std::get<Message::SharedPtr>(result);
        snowflake = msg->get_snowflake();
        auto remove_result = table.remove_message(snowflake);
        EXPECT_TRUE(std::holds_alternative<std::monostate>(remove_result));
    }
    {
        // Reload a new MessageTable from persistent storage.
        MessageTable reloadedTable(test_dir);
        auto msg_opt = reloadedTable.get_by_uid(snowflake);
        EXPECT_FALSE(msg_opt.has_value());
    }
}
