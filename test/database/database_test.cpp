#include <gtest/gtest.h>
#include "server/db/database.hpp"
#include "models/uuid.hpp"
#include "models/user.hpp"
#include "models/message.hpp"
#include "models/channel.hpp"
TEST(DatabaseTest, AddUserSuccessfully) {
    Database& db = Database::get_instance();
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    UUID userUid = user->get_uid();
    auto result = db.add_user(user, "securePass123");
    EXPECT_TRUE(std::holds_alternative<std::monostate>(result));
}

TEST(DatabaseTest, GetUserByUid) {
    Database& db = Database::get_instance();
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    db.add_user(user, "securePass123");
    auto retrievedUser = db.get_user_by_uid(user->get_uid());
    EXPECT_TRUE(retrievedUser.has_value());
}

TEST(DatabaseTest, GetMutUserByUid) {
    Database& db = Database::get_instance();
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    db.add_user(user, "securePass123");
    auto retrievedUser = db.get_mut_user_by_uid(user->get_uid());
    EXPECT_TRUE(retrievedUser.has_value());
}

TEST(DatabaseTest, RemoveUserSuccessfully) {
    Database& db = Database::get_instance();
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    db.add_user(user, "securePass123");
    auto removeResult = db.remove_user(user->get_uid());
    EXPECT_TRUE(std::holds_alternative<User::SharedPtr>(removeResult));
    auto retrievedUser = db.get_user_by_uid(user->get_uid());
    EXPECT_FALSE(retrievedUser.has_value());
}

TEST(DatabaseTest, AddChannelSuccessfully) {
    Database& db = Database::get_instance();
    UUID member1, member2;
    std::vector<UUID> members = {member1, member2};
    auto result = db.add_channel("General", members);
    EXPECT_TRUE(std::holds_alternative<Channel::SharedPtr>(result));
}

TEST(DatabaseTest, RemoveNonexistentUser) {
    Database& db = Database::get_instance();
    UUID userUid;
    auto result = db.remove_user(userUid);
    EXPECT_TRUE(std::holds_alternative<std::string>(result));
}

TEST(DatabaseTest, VerifyPassword) {
    Database& db = Database::get_instance();
    User::SharedPtr user = std::make_shared<User>("testusername", "testuser");
    db.add_user(user, "securePass123");
    UUID userUid = user->get_uid();
    auto result = db.verify_password(userUid, "securePass123");
    EXPECT_TRUE(std::holds_alternative<bool>(result));
    EXPECT_TRUE(std::get<bool>(result));
}
