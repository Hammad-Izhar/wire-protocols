#include <gtest/gtest.h>

#include "constants.hpp"
#include "message/header.hpp"
#include "message/send_message.hpp"
#include "models/message.hpp"
#include "models/user.hpp"
#include "models/channel.hpp"


TEST(SendMessage, SendsEmptyMessage) {
    User user1("username1", "display_name1");
    User user2("username2", "display_name1");
    std::vector<UUID> user_ids = {user1.get_uid(), user2.get_uid()};
    Channel channel("channel", user_ids);
    Message message(channel.get_uid(), user1.get_uid(), "");
    SendMessageMessage send_message_message(channel.get_uid(), user1.get_uid(), message.get_text());

    std::vector<uint8_t> buf;

    send_message_message.serialize_msg(buf);

    Header deserialized_header;
    SendMessageMessage deserialized_message;
    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));

    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::SEND_MESSAGE);
    EXPECT_EQ(deserialized_message.get_channel_uid(), channel.get_uid());
    EXPECT_EQ(deserialized_message.get_sender_uid(), user1.get_uid());
    EXPECT_EQ(deserialized_message.get_text(), "");
}

TEST(SendMessage, SendsMessage) {
    User user1("username1", "display_name1");
    User user2("username2", "display_name1");
    std::vector<UUID> user_ids = {user1.get_uid(), user2.get_uid()};
    Channel channel("channel", user_ids);
    Message message(channel.get_uid(), user1.get_uid(), "Hello, world!");
    SendMessageMessage send_message_message(channel.get_uid(), user1.get_uid(), message.get_text());

    std::vector<uint8_t> buf;

    send_message_message.serialize_msg(buf);

    Header deserialized_header;

    SendMessageMessage deserialized_message;
    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));
        
    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::SEND_MESSAGE);
    EXPECT_EQ(deserialized_message.get_channel_uid(), channel.get_uid());
    EXPECT_EQ(deserialized_message.get_sender_uid(), user1.get_uid());
    EXPECT_EQ(deserialized_message.get_text(), "Hello, world!");
}


TEST(SendMessage, SendsMessageWithSpecialChars) {
    User user1("username1", "display_name1");
    User user2("username2", "display_name1");
    std::vector<UUID> user_ids = {user1.get_uid(), user2.get_uid()};
    Channel channel("channel", user_ids);
    Message message(channel.get_uid(), user1.get_uid(), "Hello, world! @#$%^&*()_+[]{}|;':,.<>?/~`");
    SendMessageMessage send_message_message(channel.get_uid(), user1.get_uid(), message.get_text());

    std::vector<uint8_t> buf;

    send_message_message.serialize_msg(buf);

    Header deserialized_header;

    SendMessageMessage deserialized_message;
    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));
        
    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::SEND_MESSAGE);
    EXPECT_EQ(deserialized_message.get_channel_uid(), channel.get_uid());
    EXPECT_EQ(deserialized_message.get_sender_uid(), user1.get_uid());
    EXPECT_EQ(deserialized_message.get_text(), "Hello, world! @#$%^&*()_+[]{}|;':,.<>?/~`");
}

TEST(SendMessage, SendsLongMessage) {
    User user1("username1", "display_name1");
    User user2("username2", "display_name1");
    std::vector<UUID> user_ids = {user1.get_uid(), user2.get_uid()};
    Channel channel("channel", user_ids);
    std::string long_text = std::string(100, ' ').replace(0, 1200, std::string(100, ' ').replace(0, 12, "Hello, world!"));
    Message message(channel.get_uid(), user1.get_uid(), long_text);
    SendMessageMessage send_message_message(channel.get_uid(), user1.get_uid(), message.get_text());

    std::vector<uint8_t> buf;

    send_message_message.serialize_msg(buf);

    Header deserialized_header;

    SendMessageMessage deserialized_message;
    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));
        
    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::SEND_MESSAGE);
    EXPECT_EQ(deserialized_message.get_channel_uid(), channel.get_uid());
    EXPECT_EQ(deserialized_message.get_sender_uid(), user1.get_uid());
    EXPECT_EQ(deserialized_message.get_text(), long_text);
}