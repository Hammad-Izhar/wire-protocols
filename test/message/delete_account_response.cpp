#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include "message/delete_account_response.hpp"
#include "models/user.hpp"  // Assuming you have a User class

// Test case for creating the response with User data
TEST(DeleteAccountResponseTest, TestSerializationWithUserData) {
    // Create a mock User object
    User::SharedPtr user = std::make_shared<User>("username", "display_name");

    DeleteAccountResponse response(user);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a DeleteAccountResponse object
    DeleteAccountResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct data
    ASSERT_TRUE(deserialized_response.get_data().has_value());
    std::string username = deserialized_response.get_data().value()->get_username();
    std::string display_name = deserialized_response.get_data().value()->get_display_name();

    ASSERT_EQ(username, user->get_username());
    ASSERT_EQ(display_name, user->get_display_name());
}

// Test case for creating the response with an error message string
TEST(DeleteAccountResponseTest, TestSerializationWithErrorMessage) {
    std::string error_msg = "Error occurred during deletion";
    DeleteAccountResponse response(error_msg);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a DeleteAccountResponse object
    DeleteAccountResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct error message
    ASSERT_TRUE(deserialized_response.get_error_message().has_value());
    ASSERT_EQ(deserialized_response.get_error_message().value(), error_msg);
}

// Test case for checking the size of the serialized object
TEST(DeleteAccountResponseTest, TestSize) {
    User::SharedPtr user = std::make_shared<User>();
    DeleteAccountResponse response(user);

    std::vector<uint8_t> buf;
    response.serialize(buf);

    size_t serialized_size = response.size();
    ASSERT_GT(serialized_size, 0);  // Ensure the size is greater than 0
}

// Test case for checking the success status when there is User data
TEST(DeleteAccountResponseTest, TestIsSuccessWithUser) {
    User::SharedPtr user = std::make_shared<User>();
    DeleteAccountResponse response(user);

    // Check if is_success() returns true when User data is present
    ASSERT_TRUE(response.is_success());
}

// Test case for checking the success status when there is an error message
TEST(DeleteAccountResponseTest, TestIsSuccessWithErrorMessage) {
    DeleteAccountResponse response("Error occurred during deletion");

    // Check if is_success() returns false when error message is present
    ASSERT_FALSE(response.is_success());
}