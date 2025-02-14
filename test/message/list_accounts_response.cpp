#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include "message/list_accounts_response.hpp"  // Include the header for ListAccountsResponse
#include "models/user.hpp"            // Include User model header

// Test case for creating the response with User data
TEST(ListAccountsResponseTest, TestSerializationWithUserData) {
    // Create a mock User object
    User::SharedPtr user = std::make_shared<User>("username", "display_name"); 
    std::vector<User::SharedPtr> users = {user};

    ListAccountsResponse response(users);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a ListAccountsResponse object
    ListAccountsResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct users
    ASSERT_TRUE(deserialized_response.get_users().has_value());

    std::vector<User::SharedPtr> deserialized_users = deserialized_response.get_users().value();
    ASSERT_EQ(deserialized_users.size(), 1);  // Ensure there is only one user

    User::SharedPtr deserialized_user = deserialized_users[0];

    // Check if the deserialized user contains the correct data
    ASSERT_EQ(deserialized_user->get_username(), user->get_username());
    ASSERT_EQ(deserialized_user->get_display_name(), user->get_display_name());
}

// Test case for creating the response with an error message
TEST(ListAccountsResponseTest, TestSerializationWithErrorMessage) {
    std::string error_msg = "Error occurred while listing accounts";
    ListAccountsResponse response(error_msg);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a ListAccountsResponse object
    ListAccountsResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct error message
    ASSERT_TRUE(deserialized_response.get_error_message().has_value());
    ASSERT_EQ(deserialized_response.get_error_message().value(), error_msg);
}

// Test case for checking the size of the serialized object
TEST(ListAccountsResponseTest, TestSize) {
    User::SharedPtr user = std::make_shared<User>();
    std::vector<User::SharedPtr> users = {user};
    ListAccountsResponse response(users);

    std::vector<uint8_t> buf;
    response.serialize(buf);

    size_t serialized_size = response.size();
    ASSERT_GT(serialized_size, 0);  // Ensure the size is greater than 0
}

// Test case for checking the success status when there is User data
TEST(ListAccountsResponseTest, TestIsSuccessWithUserData) {
    User::SharedPtr user = std::make_shared<User>();
    std::vector<User::SharedPtr> users = {user};
    ListAccountsResponse response(users);

    // Check if is_success() returns true when User data is present
    ASSERT_TRUE(response.is_success());
}

// Test case for checking the success status when there is an error message
TEST(ListAccountsResponseTest, TestIsSuccessWithErrorMessage) {
    ListAccountsResponse response("Error occurred while listing accounts");

    // Check if is_success() returns false when error message is present
    ASSERT_FALSE(response.is_success());
}
