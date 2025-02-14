#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include "message/delete_message_response.hpp"
#include "models/message.hpp"  
#include "models/user.hpp"
#include "models/channel.hpp"

// Test case for creating the response with Message data
TEST(DeleteMessageResponseTest, TestSerializationWithMessageData) {
    // Create a mock Message object
    User u1("username", "display_name");
    User u2("username2", "display_name2");
    Channel channel("channel", {u1.get_uid(), u2.get_uid()});
    Message::SharedPtr message = std::make_shared<Message>(u1.get_uid(), channel.get_uid(), "text");

    DeleteMessageResponse response(message);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a DeleteMessageResponse object
    DeleteMessageResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct data
    ASSERT_TRUE(deserialized_response.get_data().has_value());

    Message::SharedPtr deserialized_message = deserialized_response.get_data().value();
    ASSERT_EQ(deserialized_message->get_sender_id(), message->get_sender_id());
    ASSERT_EQ(deserialized_message->get_channel_id(), message->get_channel_id());
    ASSERT_EQ(deserialized_message->get_text(), message->get_text());
    ASSERT_EQ(deserialized_message->get_snowflake(), message->get_snowflake());
    ASSERT_EQ(deserialized_message->get_created_at(), message->get_created_at());
}

// Test case for creating the response with an error message string
TEST(DeleteMessageResponseTest, TestSerializationWithErrorMessage) {
    std::string error_msg = "Error occurred during message deletion";
    DeleteMessageResponse response(error_msg);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a DeleteMessageResponse object
    DeleteMessageResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct error message
    ASSERT_TRUE(deserialized_response.get_error_message().has_value());
    ASSERT_EQ(deserialized_response.get_error_message().value(), error_msg);
}


// Test case for checking the size of the serialized object
TEST(DeleteMessageResponseTest, TestSize) {
    Message::SharedPtr message = std::make_shared<Message>();
    DeleteMessageResponse response(message);

    std::vector<uint8_t> buf;
    response.serialize(buf);

    size_t serialized_size = response.size();
    ASSERT_GT(serialized_size, 0);  // Ensure the size is greater than 0
}

// Test case for checking the success status when there is Message data
TEST(DeleteMessageResponseTest, TestIsSuccessWithMessage) {
    Message::SharedPtr message = std::make_shared<Message>();
    DeleteMessageResponse response(message);

    // Check if is_success() returns true when Message data is present
    ASSERT_TRUE(response.is_success());
}

