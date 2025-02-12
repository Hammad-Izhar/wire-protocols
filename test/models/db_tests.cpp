#include <gtest/gtest.h>
#include "models/user_db.hpp"
#include "models/message_db.hpp"
#include <stdexcept>

TEST(UserDbTest, AddUser) {
    user_db db;
    User user = User("thomask", "1234", "Thomas");
    EXPECT_NO_THROW(db.add_user(user));
}

TEST(UserDbTest, AddUserAndGetByUid) {
    user_db db;
    User user = User("thomask", "1234", "Thomas");
    db.add_user(user);
    EXPECT_EQ(db.get_by_uid(user.get_uid()).get_username(), "thomask");
}

TEST(UserDbTest, AddUserAndRemoveByUid) {
    user_db db;
    User user = User("thomask", "1234", "Thomas");
    db.add_user(user);
    EXPECT_NO_THROW(db.remove_user(user.get_uid()));
}

TEST(MessageDBTest, AddMessage) {
    message_db db;
    UUID sender = UUID();
    UUID channel = UUID();
    Message message = Message(sender, channel, "Hello World");
    EXPECT_NO_THROW(db.add_message(message));
}

TEST(MessageDBTest, AddMessageAndGetByUid) {
    message_db db;
    UUID sender = UUID();
    UUID channel = UUID();
    Message message = Message(sender, channel, "Hello World");
    db.add_message(message);
    EXPECT_EQ(db.get_by_uid(message.get_snowflake()).get_text(), "Hello World");
}

TEST(MessageDBTest, AddMessageAndRemoveByUid) {
    message_db db;
    UUID sender = UUID();
    UUID channel = UUID();
    Message message = Message(sender, channel, "Hello World");
    db.add_message(message);
    EXPECT_NO_THROW(db.remove_message(message.get_snowflake()));
}