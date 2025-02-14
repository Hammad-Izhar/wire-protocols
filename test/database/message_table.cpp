#include <gtest/gtest.h>
#include <unordered_map>
#include <string>
#include <optional>
#include "server/db/message_table.hpp"
#include "models/message.hpp"
#include "models/uuid.hpp"

UUID sender_uid;
UUID channel_uid;

// Test case for adding a message and retrieving it
TEST(MessageTableTest, TestAddMessage) {
    MessageTable table;

    // Add a message
    auto result = table.add_message(sender_uid, channel_uid, "Test message content");
    
    // Ensure the result is a valid Message pointer (no error)
    ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(result));

    // Retrieve the message by its snowflake
    auto snowflake = std::get<Message::SharedPtr>(result)->get_snowflake();
    auto retrieved_message = table.get_by_uid(snowflake);

    ASSERT_TRUE(retrieved_message.has_value());
    ASSERT_EQ(retrieved_message.value()->get_snowflake(), snowflake);
    ASSERT_EQ(retrieved_message.value()->get_text(), "Test message content");
}

// Test case for trying to retrieve a message that does not exist
TEST(MessageTableTest, TestGetNonExistentMessage) {
    MessageTable table;
    
    // Try to retrieve a non-existent message
    auto retrieved_message = table.get_by_uid(12345);  // Random snowflake

    ASSERT_FALSE(retrieved_message.has_value());  // Should be empty (no message found)
}

// Test case for modifying a message using the mutable getter
TEST(MessageTableTest, TestGetMutableMessage) {
    MessageTable table;

    // Add a message
    auto result = table.add_message(sender_uid, channel_uid, "Original message");
    auto snowflake = std::get<Message::SharedPtr>(result)->get_snowflake();

    // Retrieve the message as mutable and modify its content
    auto mutable_message = table.get_mut_by_uid(snowflake);
    ASSERT_TRUE(mutable_message.has_value());

    std::string new_content = "Modified message content";
    
    mutable_message.value()->set_text(new_content);

    // Retrieve the modified message
    auto updated_message = table.get_by_uid(snowflake);
    ASSERT_TRUE(updated_message.has_value());
    ASSERT_EQ(updated_message.value()->get_text(), "Modified message content");
}

// Test case for removing a message that exists
TEST(MessageTableTest, TestRemoveMessage) {
    MessageTable table;

    // Add a message
    auto result = table.add_message(sender_uid, channel_uid, "Message to be removed");
    auto snowflake = std::get<Message::SharedPtr>(result)->get_snowflake();

    // Remove the message
    auto remove_result = table.remove_message(snowflake);
    ASSERT_TRUE(std::holds_alternative<std::monostate>(remove_result));  // No error, removal success

    // Ensure the message is no longer retrievable
    auto retrieved_message = table.get_by_uid(snowflake);
    ASSERT_FALSE(retrieved_message.has_value());  // Message should be removed
}

// Test case for thread safety when modifying and removing messages concurrently
TEST(MessageTableTest, TestThreadSafetyModifyAndRemove) {
    MessageTable table;

    // Add a message first
    auto result = table.add_message(sender_uid, channel_uid, "Message to be modified and removed");
    auto snowflake = std::get<Message::SharedPtr>(result)->get_snowflake();

    // Create threads to modify and remove the message
    std::thread modify_thread([&table, snowflake]() {
        auto mutable_message = table.get_mut_by_uid(snowflake);
        if (mutable_message.has_value()) {
            std::string modified_text = "Modified by thread";
            mutable_message.value()->set_text(modified_text);
        }
    });

    std::thread remove_thread([&table, snowflake]() {
        table.remove_message(snowflake);
    });

    // Wait for both threads to finish
    modify_thread.join();
    remove_thread.join();

    // Ensure the message is removed, and the modification did not persist
    auto retrieved_message = table.get_by_uid(snowflake);
    ASSERT_FALSE(retrieved_message.has_value());  // Message should be removed
}


