#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "message/create_channel_response.hpp"
#include "models/channel.hpp"  
#include "models/user.hpp"     

// Test case for creating the response with Channel data
TEST(CreateChannelResponseTest, TestSerializationWithChannelData) {
    User user1("username1", "display_name1");
    User user2("username2", "display_name2");
    std::vector<UUID> user_ids = {user1.get_uid(), user2.get_uid()};
    Channel::SharedPtr channel = std::make_shared<Channel>("mychannel", user_ids); // Use an appropriate Channel object
    CreateChannelResponse response(channel);

    // Serialize
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize
    CreateChannelResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the original and deserialized object are the same
    ASSERT_TRUE(deserialized_response.get_data().has_value());
    ASSERT_FALSE(deserialized_response.get_error_message().has_value());
    ASSERT_EQ(deserialized_response.get_data().value()->get_name(), channel->get_name());
    ASSERT_EQ(deserialized_response.get_data().value()->get_user_uids().size(), channel->get_user_uids().size());
    ASSERT_EQ(deserialized_response.get_data().value()->get_user_uids()[0], channel->get_user_uids()[0]);
    ASSERT_EQ(deserialized_response.get_data().value()->get_user_uids()[1], channel->get_user_uids()[1]);


}

// Test case for creating the response with error string data
TEST(CreateChannelResponseTest, TestSerializationWithErrorMessage) {
    std::string error_msg = "Some error occurred";
    CreateChannelResponse response(error_msg);

    // Serialize
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize
    CreateChannelResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized object contains the error message
    ASSERT_TRUE(deserialized_response.get_error_message().has_value());
    ASSERT_EQ(deserialized_response.get_error_message().value(), error_msg);
}

// Test case for testing size function
TEST(CreateChannelResponseTest, TestSize) {
    Channel::SharedPtr channel = std::make_shared<Channel>();
    CreateChannelResponse response(channel);

    std::vector<uint8_t> buf;
    response.serialize(buf);

    size_t serialized_size = response.size();
    ASSERT_GT(serialized_size, 0); // Size should be greater than 0
}


