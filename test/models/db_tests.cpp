#include <gtest/gtest.h>

#include "server/db/message_table.hpp"
#include "server/db/user_table.hpp"

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

TEST(MessageDBTest, AddMessage) {
    MessageTable db;
    UUID sender = UUID();
    UUID channel = UUID();
    EXPECT_NO_THROW(db.add_message(sender, channel, "Hello World"));
}

TEST(MessageDBTest, AddMessageAndGetByUid) {
    MessageTable db;
    UUID sender = UUID();
    UUID channel = UUID();
    auto message = db.add_message(sender, channel, "Hello World");
    auto message_opt = db.get_by_uid(std::get<Message::SharedPtr>(message)->get_snowflake());
    ASSERT_TRUE(message_opt.has_value());
    Message::SharedPtr message_ref = message_opt.value();
    EXPECT_EQ(message_ref->get_text(), "Hello World");
}

TEST(MessageDBTest, AddMessageAndRemoveByUid) {
    MessageTable db;
    UUID sender = UUID();
    UUID channel = UUID();
    auto message = db.add_message(sender, channel, "Hello World");
    auto message_opt = db.get_by_uid(std::get<Message::SharedPtr>(message)->get_snowflake());
    ASSERT_TRUE(message_opt.has_value());
    Message::SharedPtr message_ref = message_opt.value();
    EXPECT_NO_THROW(db.remove_message(message_ref->get_snowflake()));
}