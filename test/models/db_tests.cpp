#include <gtest/gtest.h>
#include <filesystem>
#include "server/db/message_table.hpp"
#include "models/uuid.hpp"

// Test fixture that creates and removes a temporary directory.
class MessageDBTestFixture : public ::testing::Test {
protected:
    std::string test_dir = "test_dir";
    
    void SetUp() override {
        // Remove existing directory if present, then create a fresh one.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
        std::filesystem::create_directory(test_dir);
    }
    
    void TearDown() override {
        // Clean up by removing the test directory.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(MessageDBTestFixture, AddMessage) {
    MessageTable db(test_dir);
    UUID sender = UUID();
    UUID channel = UUID();
    EXPECT_NO_THROW(db.add_message(sender, channel, "Hello World"));
}

TEST_F(MessageDBTestFixture, AddMessageAndGetByUid) {
    MessageTable db(test_dir);
    UUID sender = UUID();
    UUID channel = UUID();
    
    auto message = db.add_message(sender, channel, "Hello World");
    // Ensure the variant holds a Message::SharedPtr.
    ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(message));
    auto msg_ptr = std::get<Message::SharedPtr>(message);
    
    auto message_opt = db.get_by_uid(msg_ptr->get_snowflake());
    ASSERT_TRUE(message_opt.has_value());
    Message::SharedPtr message_ref = message_opt.value();
    EXPECT_EQ(message_ref->get_text(), "Hello World");
}

TEST_F(MessageDBTestFixture, AddMessageAndRemoveByUid) {
    MessageTable db(test_dir);
    UUID sender = UUID();
    UUID channel = UUID();
    
    auto message = db.add_message(sender, channel, "Hello World");
    ASSERT_TRUE(std::holds_alternative<Message::SharedPtr>(message));
    auto msg_ptr = std::get<Message::SharedPtr>(message);
    
    auto message_opt = db.get_by_uid(msg_ptr->get_snowflake());
    ASSERT_TRUE(message_opt.has_value());
    Message::SharedPtr message_ref = message_opt.value();
    
    EXPECT_NO_THROW(db.remove_message(message_ref->get_snowflake()));
}
