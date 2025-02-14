#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <variant>
#include "message/register_account_response.hpp" 

// Test case for creating the response with no error message (success case)
TEST(RegisterAccountResponseTest, TestSerializationWithNoErrorMessage) {
    RegisterAccountResponse response((std::monostate()));  // No error, success case

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a RegisterAccountResponse object
    RegisterAccountResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check that there is no error message (success)
    ASSERT_FALSE(deserialized_response.get_error_message().has_value());
    ASSERT_TRUE(deserialized_response.is_success());
}

// Test case for creating the response with an error message
TEST(RegisterAccountResponseTest, TestSerializationWithErrorMessage) {
    std::string error_msg = "Account registration failed due to invalid input";
    RegisterAccountResponse response(error_msg);

    // Serialize the response into a byte buffer
    std::vector<uint8_t> buf;
    response.serialize(buf);

    // Deserialize the data back into a RegisterAccountResponse object
    RegisterAccountResponse deserialized_response;
    deserialized_response.deserialize(buf);

    // Check if the deserialized response contains the correct error message
    ASSERT_TRUE(deserialized_response.get_error_message().has_value());
    ASSERT_EQ(deserialized_response.get_error_message().value(), error_msg);
}

// Test case for checking the size of the serialized object
TEST(RegisterAccountResponseTest, TestSize) {
    RegisterAccountResponse response((std::monostate()));  // No error, success case

    std::vector<uint8_t> buf;
    response.serialize(buf);

    size_t serialized_size = response.size();
    ASSERT_GT(serialized_size, 0);  // Ensure the size is greater than 0
}

// Test case for checking the success status when there is no error message
TEST(RegisterAccountResponseTest, TestIsSuccessWithNoErrorMessage) {
    RegisterAccountResponse response((std::monostate()));  // No error, success case

    // Check if is_success() returns true when there is no error message
    ASSERT_TRUE(response.is_success());
}

// Test case for checking the success status when there is an error message
TEST(RegisterAccountResponseTest, TestIsSuccessWithErrorMessage) {
    RegisterAccountResponse response("Account registration failed due to invalid input");

    // Check if is_success() returns false when error message is present
    ASSERT_FALSE(response.is_success());
}
