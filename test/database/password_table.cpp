#include <gtest/gtest.h>
#include "server/db/password_table.hpp"
#include "models/uuid.hpp"
#include <filesystem>

// Fixture for persistent PasswordTable tests.
class PasswordTableTestFixture : public ::testing::Test {
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
        // Clean up the test directory.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(PasswordTableTestFixture, AddPasswordSuccessfully) {
    PasswordTable passwordTable(test_dir);
    UUID user1;
    auto result = passwordTable.add_password(user1, "securePass123");
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

TEST_F(PasswordTableTestFixture, VerifyCorrectPassword) {
    PasswordTable passwordTable(test_dir);
    UUID user1;
    passwordTable.add_password(user1, "correctPass");
    auto result = passwordTable.verify_password(user1, "correctPass");
    EXPECT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_TRUE(std::get<bool>(result));
}

TEST_F(PasswordTableTestFixture, VerifyIncorrectPassword) {
    PasswordTable passwordTable(test_dir);
    UUID user1;
    passwordTable.add_password(user1, "correctPass");
    auto result = passwordTable.verify_password(user1, "wrongPass");
    EXPECT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_FALSE(std::get<bool>(result));
}

TEST_F(PasswordTableTestFixture, RemovePasswordSuccessfully) {
    PasswordTable passwordTable(test_dir);
    UUID user1;
    passwordTable.add_password(user1, "securePass123");
    auto result = passwordTable.remove_password(user1);
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
    
    auto verifyResult = passwordTable.verify_password(user1, "securePass123");
    EXPECT_TRUE(std::holds_alternative<std::string>(verifyResult));
}

TEST_F(PasswordTableTestFixture, RemoveNonexistentPassword) {
    PasswordTable passwordTable(test_dir);
    UUID user1;
    UUID user2;
    passwordTable.add_password(user1, "securePass123");
    auto result = passwordTable.remove_password(user2);
    // Verify that user1's password is still valid.
    std::variant<bool, std::string> verification = passwordTable.verify_password(user1, "securePass123");
    EXPECT_TRUE(std::holds_alternative<bool>(verification));
    EXPECT_TRUE(std::get<bool>(verification));
}

TEST_F(PasswordTableTestFixture, VerifyNonexistentUser) {
    PasswordTable passwordTable(test_dir);
    UUID user1;
    auto result = passwordTable.verify_password(user1, "somePass");
    // Expect an error message (std::string) for a nonexistent user.
    EXPECT_TRUE(std::holds_alternative<std::string>(result));
}
