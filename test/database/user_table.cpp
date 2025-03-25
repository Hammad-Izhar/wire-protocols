#include <gtest/gtest.h>
#include "server/db/user_table.hpp"
#include "server/db/database.hpp"
#include <regex>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

// Fixture for UserTable tests.
class UserTableTestFixture : public ::testing::Test {
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
        // Clean up test directory.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(UserTableTestFixture, AddUser) {
    UserTable db(test_dir);
    User::SharedPtr user = std::make_shared<User>("thomask", "Thomas");
    EXPECT_NO_THROW(db.add_user(user));
}

TEST_F(UserTableTestFixture, AddUserAndGetByUid) {
    UserTable db(test_dir);
    User::SharedPtr user = std::make_shared<User>("thomask", "Thomas");
    db.add_user(user);
    auto user_opt = db.get_by_uid(user->get_uid());
    ASSERT_TRUE(user_opt.has_value());
    User::SharedPtr user_ref = user_opt.value();
    EXPECT_EQ(user_ref->get_username(), "thomask");
}

TEST_F(UserTableTestFixture, AddUserAndRemoveByUid) {
    UserTable db(test_dir);
    User::SharedPtr user = std::make_shared<User>("thomask", "Thomas");
    db.add_user(user);
    auto user_opt = db.get_by_uid(user->get_uid());
    ASSERT_TRUE(user_opt.has_value());
    User::SharedPtr user_ref = user_opt.value();
    EXPECT_NO_THROW(db.remove_user(user_ref->get_uid()));
    auto removed = db.get_by_uid(user_ref->get_uid());
    EXPECT_FALSE(removed.has_value());
}

TEST_F(UserTableTestFixture, GetUsersMatchingRegex) {
    UserTable db(test_dir);
    User::SharedPtr user1 = std::make_shared<User>("thomask", "Thomas");
    User::SharedPtr user2 = std::make_shared<User>("thomas", "Tom");
    User::SharedPtr user3 = std::make_shared<User>("tom", "Tommy");
    db.add_user(user1);
    db.add_user(user2);
    db.add_user(user3);
    auto user_uiids = db.get_uuids_matching_regex("thomas|thomask");
    ASSERT_TRUE(std::holds_alternative<std::vector<UUID>>(user_uiids));
    std::vector<UUID> uuids = std::get<std::vector<UUID>>(user_uiids);
    EXPECT_EQ(uuids.size(), 2);
}

TEST_F(UserTableTestFixture, HandlesInvalidRegex) {
    UserTable db(test_dir);
    User::SharedPtr user1 = std::make_shared<User>("thomask", "Thomas");
    User::SharedPtr user2 = std::make_shared<User>("thomas", "Tom");
    User::SharedPtr user3 = std::make_shared<User>("tom", "Tommy");
    db.add_user(user1);
    db.add_user(user2);
    db.add_user(user3);
    auto user_uiids = db.get_uuids_matching_regex("[");
    ASSERT_TRUE(std::holds_alternative<std::string>(user_uiids));
    std::string error = std::get<std::string>(user_uiids);
    EXPECT_EQ(error, "Regex error: Unexpected character within '[...]' in regular expression");

    user_uiids = db.get_uuids_matching_regex("[a-z");
    ASSERT_TRUE(std::holds_alternative<std::string>(user_uiids));
    error = std::get<std::string>(user_uiids);
    EXPECT_EQ(error, "Regex error: Unexpected character within '[...]' in regular expression");
}
