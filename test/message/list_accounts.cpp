#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "message/list_accounts.hpp"  // Include the header file for ListAccountsMessage

// Test case for creating the message with a regex string
TEST(ListAccountsMessageTest, TestSerializationWithRegex) {
    std::string regex = ".*test.*";
    ListAccountsMessage message(regex);

    // Serialize the message into a byte buffer
    std::vector<uint8_t> buf;
    message.serialize(buf);

    // Deserialize the data back into a ListAccountsMessage object
    ListAccountsMessage deserialized_message;
    deserialized_message.deserialize(buf);

    // Check if the deserialized message contains the correct regex string
    ASSERT_EQ(deserialized_message.get_regex(), regex);
}

// Test case for checking the size of the serialized object
TEST(ListAccountsMessageTest, TestSize) {
    std::string regex = ".*test.*";
    ListAccountsMessage message(regex);

    std::vector<uint8_t> buf;
    message.serialize(buf);

    size_t serialized_size = message.size();
    ASSERT_GT(serialized_size, 0);  // Ensure the size is greater than 0
}

// Test case for checking the regex getter and setter
TEST(ListAccountsMessageTest, TestGetSetRegex) {
    std::string regex = ".*account.*";
    ListAccountsMessage message;
    message.set_regex(regex);

    // Check if the regex was correctly set and retrieved
    ASSERT_EQ(message.get_regex(), regex);
}

// Test case for serializing and deserializing with an empty regex
TEST(ListAccountsMessageTest, TestSerializationWithEmptyRegex) {
    ListAccountsMessage message("");  // Create a message with an empty regex

    // Serialize the message into a byte buffer
    std::vector<uint8_t> buf;
    message.serialize(buf);

    // Deserialize the data back into a ListAccountsMessage object
    ListAccountsMessage deserialized_message;
    deserialized_message.deserialize(buf);

    // Check if the deserialized message contains the empty regex string
    ASSERT_EQ(deserialized_message.get_regex(), "");
}

// Test case for checking empty object size (without setting regex)
TEST(ListAccountsMessageTest, TestSizeWithEmptyRegex) {
    ListAccountsMessage message;

    std::vector<uint8_t> buf;
    message.serialize(buf);

    size_t serialized_size = message.size();
    ASSERT_GT(serialized_size, 0);  // Ensure the size is greater than 0, even with an empty regex
}
