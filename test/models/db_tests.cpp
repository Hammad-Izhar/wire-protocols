#include <gtest/gtest.h>

#include "server/db/message_table.hpp"


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
