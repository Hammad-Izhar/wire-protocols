#include <gtest/gtest.h>
#include <stdexcept>

#include "models/message.hpp"

TEST(MessageTest, MakeMessage)
{
    UUID sender_id = UUID();
    UUID channel_id = UUID();
    std::string text = "Hello world";
    Message message(sender_id, channel_id, text);

    EXPECT_EQ(message.get_sender_id(), sender_id);
    EXPECT_EQ(message.get_channel_id(), channel_id);
    EXPECT_EQ(message.get_text(), text);
}

TEST(MessageTest, ModifyMessage)
{
    UUID sender_id = UUID();
    UUID channel_id = UUID();
    std::string text = "Hello world";
    Message message(sender_id, channel_id, text);
    time_t modified_at = message.get_modified_at();

    sleep(1);

    std::string new_text = "Goodbye world!";
    message.set_text(new_text);
    EXPECT_EQ(message.get_text(), new_text);

    // Check that modified time is updated
    EXPECT_GT(message.get_modified_at(), modified_at);
}

TEST(MessageTest, UniqueSnowflakes)
{
    UUID sender_id = UUID();
    UUID channel_id = UUID();
    std::string text = "Hello world";
    Message message1(sender_id, channel_id, text);
    Message message2(sender_id, channel_id, text);
    EXPECT_NE(message1.get_snowflake(), message2.get_snowflake());
}

TEST(MessageTest, AddReadBy)
{
    UUID sender_id = UUID();
    UUID channel_id = UUID();
    std::string text = "Hello world";
    Message message(sender_id, channel_id, text);
    UUID user_id = UUID();
    message.set_read_by(user_id);
    EXPECT_EQ(message.get_read_by().size(), 2);
}
