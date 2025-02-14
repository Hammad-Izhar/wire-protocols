#include <gtest/gtest.h>
#include "server/db/user_table.hpp"
#include "server/db/database.hpp"
#include <regex>

TEST(UserDbTest, AddUser) {
    UserTable db;
    User::SharedPtr user = std::make_shared<User>("thomask", "Thomas");
    EXPECT_NO_THROW(db.add_user(user));
}

TEST(UserDbTest, AddUserAndGetByUid) {
    UserTable db;
    User::SharedPtr user = std::make_shared<User>("thomask", "Thomas");
    db.add_user(user);
    auto user_opt = db.get_by_uid(user->get_uid());
    ASSERT_TRUE(user_opt.has_value());
    User::SharedPtr user_ref = user_opt.value();
    EXPECT_EQ(user_ref->get_username(), "thomask");
}

TEST(UserDbTest, AddUserAndRemoveByUid) {
    UserTable db;
    User::SharedPtr user = std::make_shared<User>("thomask", "Thomas");
    db.add_user(user);
    auto user_opt = db.get_by_uid(user->get_uid());
    ASSERT_TRUE(user_opt.has_value());
    User::SharedPtr user_ref = user_opt.value();
    EXPECT_NO_THROW(db.remove_user(user_ref->get_uid()));
}

TEST(UserDBTest, GetUsersMatchingRegex) {
    UserTable db;
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

TEST(UserDBTest, HandlesInvalidRegex) {
    UserTable db;
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