#include <gtest/gtest.h>
#include <stdexcept>

#include "server/db/user_table.hpp"
#include "server/db/message_table.hpp"

TEST(UserDbTest, AddUser)
{
    UserTable db;
    User user = User("thomask", "Thomas");
    EXPECT_NO_THROW(db.add_user(user));
}

TEST(UserDbTest, AddUserAndGetByUid)
{
    UserTable db;
    User user = User("thomask", "Thomas");
    db.add_user(user);
    EXPECT_EQ(db.get_by_uid(user.get_uid()).get_username(), "thomask");
}

TEST(UserDbTest, AddUserAndRemoveByUid)
{
    UserTable db;
    User user = User("thomask", "Thomas");
    db.add_user(user);
    EXPECT_NO_THROW(db.remove_user(user.get_uid()));
}

TEST(MessageDBTest, AddMessage)
{
    MessageTable db;
    UUID sender = UUID();
    UUID channel = UUID();
    Message message = Message(sender, channel, "Hello World");
    EXPECT_NO_THROW(db.add_message(message));
}

TEST(MessageDBTest, AddMessageAndGetByUid)
{
    MessageTable db;
    UUID sender = UUID();
    UUID channel = UUID();
    Message message = Message(sender, channel, "Hello World");
    db.add_message(message);
    EXPECT_EQ(db.get_by_uid(message.get_snowflake()).get_text(), "Hello World");
}

TEST(MessageDBTest, AddMessageAndRemoveByUid)
{
    MessageTable db;
    UUID sender = UUID();
    UUID channel = UUID();
    Message message = Message(sender, channel, "Hello World");
    db.add_message(message);
    EXPECT_NO_THROW(db.remove_message(message.get_snowflake()));
}