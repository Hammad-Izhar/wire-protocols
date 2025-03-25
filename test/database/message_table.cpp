#include <gtest/gtest.h>
#include "server/db/message_table.hpp"
#include "models/message.hpp"
#include "models/uuid.hpp"
#include <filesystem>
#include <thread>

// Fixture for MessageTable tests using persistent storage.
class MessageTableTestFixture : public ::testing::Test {
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
        // Remove the test directory after each test.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(MessageTableTestFixture, TestAddMessage) {
    MessageTable table(test_dir);
    UUID sender_uid;
    UUID channel_uid;
    
    // Add a message.
    auto result = table.add_message(sender_uid, channel_uid, "Test message content");
    ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(result));
    
    // Retrieve the message by its snowflake.
    auto snowflake = std::get<Message::SharedPtr>(result)->get_snowflake();
    auto retrieved_message = table.get_by_uid(snowflake);
    ASSERT_TRUE(retrieved_message.has_value());
    ASSERT_EQ(retrieved_message.value()->get_snowflake(), snowflake);
    ASSERT_EQ(retrieved_message.value()->get_text(), "Test message content");
}

TEST_F(MessageTableTestFixture, TestGetNonExistentMessage) {
    MessageTable table(test_dir);
    // Try to retrieve a non-existent message (using an arbitrary snowflake).
    auto retrieved_message = table.get_by_uid(12345);
    ASSERT_FALSE(retrieved_message.has_value());
}

TEST_F(MessageTableTestFixture, TestGetMutableMessage) {
    MessageTable table(test_dir);
    UUID sender_uid;
    UUID channel_uid;
    
    // Add a message.
    auto result = table.add_message(sender_uid, channel_uid, "Original message");
    ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(result));
    auto snowflake = std::get<Message::SharedPtr>(result)->get_snowflake();
    
    // Retrieve the message as mutable and modify its content.
    auto mutable_message = table.get_mut_by_uid(snowflake);
    ASSERT_TRUE(mutable_message.has_value());
    std::string new_content = "Modified message content";
    mutable_message.value()->set_text(new_content);
    
    // Retrieve the updated message.
    auto updated_message = table.get_by_uid(snowflake);
    ASSERT_TRUE(updated_message.has_value());
    ASSERT_EQ(updated_message.value()->get_text(), "Modified message content");
}

TEST_F(MessageTableTestFixture, TestRemoveMessage) {
    MessageTable table(test_dir);
    UUID sender_uid;
    UUID channel_uid;
    
    // Add a message.
    auto result = table.add_message(sender_uid, channel_uid, "Message to be removed");
    ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(result));
    auto snowflake = std::get<Message::SharedPtr>(result)->get_snowflake();
    
    // Remove the message.
    auto remove_result = table.remove_message(snowflake);
    ASSERT_TRUE(std::holds_alternative<std::monostate>(remove_result));
    
    // Verify the message is no longer retrievable.
    auto retrieved_message = table.get_by_uid(snowflake);
    ASSERT_FALSE(retrieved_message.has_value());
}

// TEST_F(MessageTableTestFixture, TestThreadSafetyModifyAndRemove) {
//     MessageTable table(test_dir);
//     UUID sender_uid;
//     UUID channel_uid;
    
//     // Add a message.
//     auto result = table.add_message(sender_uid, channel_uid, "Message to be modified and removed");
//     ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(result));
//     auto snowflake = std::get<Message::SharedPtr>(result)->get_snowflake();
    
//     // Launch two threads: one modifies the message and one removes it.
//     std::thread modify_thread([&table, snowflake]() {
//         auto mutable_message = table.get_mut_by_uid(snowflake);
//         if (mutable_message.has_value()) {
//             mutable_message.value()->set_text("Modified by thread");
//         }
//     });
    
//     std::thread remove_thread([&table, snowflake]() {
//         table.remove_message(snowflake);
//     });
    
//     modify_thread.join();
//     remove_thread.join();
    
//     // Verify that the message is removed (modification should not persist).
//     auto retrieved_message = table.get_by_uid(snowflake);
//     ASSERT_FALSE(retrieved_message.has_value());
// }
