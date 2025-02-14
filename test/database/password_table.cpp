#include <gtest/gtest.h>
#include "server/db/password_table.hpp"
#include "models/uuid.hpp"

TEST(AddPasswordSuccessfully, PasswordTableTest) {
    PasswordTable passwordTable;
    UUID user1;
    auto result = passwordTable.add_password(user1, "securePass123");
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

TEST(VerifyCorrectPassword, PasswordTableTest) {
    PasswordTable passwordTable;
    UUID user1;
    passwordTable.add_password(user1, "correctPass");
    auto result = passwordTable.verify_password(user1, "correctPass");
    EXPECT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_TRUE(std::get<bool>(result));
}

TEST(VerifyIncorrectPassword, PasswordTableTest) {
    PasswordTable passwordTable;
    UUID user1;
    passwordTable.add_password(user1, "correctPass");
    auto result = passwordTable.verify_password(user1, "wrongPass");
    EXPECT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_FALSE(std::get<bool>(result));
}

TEST(RemovePasswordSuccessfully, PasswordTableTest) {
    PasswordTable passwordTable;
    UUID user1;
    passwordTable.add_password(user1, "securePass123");
    auto result = passwordTable.remove_password(user1);
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
    
    auto verifyResult = passwordTable.verify_password(user1, "securePass123");
    EXPECT_TRUE(std::holds_alternative<std::string>(verifyResult));
}

TEST(RemoveNonexistentPassword, PasswordTableTest) {
    PasswordTable passwordTable;
    UUID user1;
    UUID user2;
    passwordTable.add_password(user1, "securePass123");
    auto result = passwordTable.remove_password(user2);
    // Expect the number of elements in the data map to be unchanged
    std::variant<bool, std::string> verification = passwordTable.verify_password(user1, "securePass123");
    EXPECT_TRUE(std::holds_alternative<bool>(verification));
    EXPECT_TRUE(std::get<bool>(verification));
}

TEST(VerifyNonexistentUser, PasswordTableTest) {
    PasswordTable passwordTable;
    UUID user1;
    auto result = passwordTable.verify_password(user1, "somePass");
}
