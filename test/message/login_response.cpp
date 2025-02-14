#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>
#include "message/login_response.hpp"  // Include the header for LoginResponse
#include "models/user.hpp"     // Include User model header

// Test case for creating the response with User data
TEST(LoginResponseTest, TestSerializationWithUserData) {
    // Create a mock User object
    User::SharedPtr user = std::make_shared<User>("username", "display_name");
    LoginResponse response(user);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a LoginResponse object
    LoginResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct user
    ASSERT_TRUE(deserialized_response.get_data().has_value());

    User::SharedPtr deserialized_user = deserialized_response.get_data().value();
    ASSERT_EQ(deserialized_user->get_username(), user->get_username());
    ASSERT_EQ(deserialized_user->get_display_name(), user->get_display_name());
}

// Test case for creating the response with an error message
TEST(LoginResponseTest, TestSerializationWithErrorMessage) {
    std::string error_msg = "Login failed due to invalid credentials";
    LoginResponse response(error_msg);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a LoginResponse object
    LoginResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct error message
    ASSERT_TRUE(deserialized_response.get_error_message().has_value());
    ASSERT_EQ(deserialized_response.get_error_message().value(), error_msg);
}

// Test case for checking the size of the serialized object
TEST(LoginResponseTest, TestSize) {
    User::SharedPtr user = std::make_shared<User>();
    LoginResponse response(user);

    std::vector<uint8_t> buf;
    response.serialize(buf);

    size_t serialized_size = response.size();
    ASSERT_GT(serialized_size, 0);  // Ensure the size is greater than 0
}

// Test case for checking the success status when there is User data
TEST(LoginResponseTest, TestIsSuccessWithUserData) {
    User::SharedPtr user = std::make_shared<User>();
    LoginResponse response(user);

    // Check if is_success() returns true when User data is present
    ASSERT_TRUE(response.is_success());
}

// Test case for checking the success status when there is an error message
TEST(LoginResponseTest, TestIsSuccessWithErrorMessage) {
    LoginResponse response("Login failed due to invalid credentials");

    // Check if is_success() returns false when error message is present
    ASSERT_FALSE(response.is_success());
}