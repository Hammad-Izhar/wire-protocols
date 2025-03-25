#include <gtest/gtest.h>
#include "server/db/database.hpp"
#include "models/uuid.hpp"
#include "models/user.hpp"
#include "models/message.hpp"
#include "models/channel.hpp"
#include <filesystem>

// Fixture for Database tests using a persistent directory.
class DatabaseTestFixture : public ::testing::Test {
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
        // Remove the test directory after tests.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(DatabaseTestFixture, AddUserSuccessfully) {
    Database db(test_dir);
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    UUID userUid = user->get_uid();
    auto result = db.add_user(user, "securePass123");
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

TEST_F(DatabaseTestFixture, GetUserByUid) {
    Database db(test_dir);
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    db.add_user(user, "securePass123");
    auto retrievedUser = db.get_user_by_uid(user->get_uid());
    EXPECT_TRUE(retrievedUser.has_value());
}

TEST_F(DatabaseTestFixture, GetMutUserByUid) {
    Database db(test_dir);
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    db.add_user(user, "securePass123");
    auto retrievedUser = db.get_mut_user_by_uid(user->get_uid());
    EXPECT_TRUE(retrievedUser.has_value());
}

TEST_F(DatabaseTestFixture, RemoveUserSuccessfully) {
    Database db(test_dir);
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    db.add_user(user, "securePass123");
    auto removeResult = db.remove_user(user->get_uid());
    EXPECT_TRUE(std::holds_alternative<User::SharedPtr>(removeResult));
    auto retrievedUser = db.get_user_by_uid(user->get_uid());
    EXPECT_FALSE(retrievedUser.has_value());
}

TEST_F(DatabaseTestFixture, AddChannelSuccessfully) {
    Database db(test_dir);
    UUID member1, member2;
    std::vector<UUID> members = {member1, member2};
    auto result = db.add_channel("General", members);
    EXPECT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
}

TEST_F(DatabaseTestFixture, RemoveNonexistentUser) {
    Database db(test_dir);
    UUID userUid;
    auto result = db.remove_user(userUid);
    EXPECT_TRUE(std::holds_alternative<std::string>(result));
}

TEST_F(DatabaseTestFixture, VerifyPassword) {
    Database db(test_dir);
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    db.add_user(user, "securePass123");
    UUID userUid = user->get_uid();
    auto result = db.verify_password(userUid, "securePass123");
    EXPECT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_TRUE(std::get<bool>(result));
}
