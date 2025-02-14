#include <gtest/gtest.h>

#include "constants.hpp"
#include "message/header.hpp"
#include "message/create_channel.hpp"
#include "models/channel.hpp"
#include "models/user.hpp"


TEST(CreateChannel, CreatesEmptyChannel) {
    std::vector<UUID> user_ids;
    Channel channel("channel", user_ids);
    CreateChannelMessage create_channel_message(channel.get_name(), channel.get_user_uids());

    std::vector<uint8_t> buf;

    create_channel_message.serialize_msg(buf);

    Header deserialized_header;
    CreateChannelMessage deserialized_message;
    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));

    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::CREATE_CHANNEL);
    EXPECT_EQ(deserialized_message.get_channel_name(), "channel");
    EXPECT_EQ(deserialized_message.get_members().size(), 0);
}

TEST(CreateChannel, CreatesChannel) {
    User user1("username1", "display_name1");
    User user2("username2", "display_name1");
    std::vector<UUID> user_ids = {user1.get_uid(), user2.get_uid()};
    Channel channel("channel", user_ids);
    CreateChannelMessage create_channel_message(channel.get_name(), channel.get_user_uids());

    std::vector<uint8_t> buf;

    create_channel_message.serialize_msg(buf);

    Header deserialized_header;
    CreateChannelMessage deserialized_message;
    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));

    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::CREATE_CHANNEL);
    EXPECT_EQ(deserialized_message.get_channel_name(), "channel");
    EXPECT_EQ(deserialized_message.get_members().size(), 2);
    EXPECT_EQ(deserialized_message.get_members()[0], user1.get_uid());
    EXPECT_EQ(deserialized_message.get_members()[1], user2.get_uid());
}

TEST(CreateChannel, CreatesChannelWithOneUser) {
    User user1("username1", "display_name1");
    std::vector<UUID> user_ids = {user1.get_uid()};
    Channel channel("channel", user_ids);
    CreateChannelMessage create_channel_message(channel.get_name(), channel.get_user_uids());

    std::vector<uint8_t> buf;
    
    create_channel_message.serialize_msg(buf);

    Header deserialized_header;
    CreateChannelMessage deserialized_message;

    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));

    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::CREATE_CHANNEL);
    EXPECT_EQ(deserialized_message.get_channel_name(), "channel");
    EXPECT_EQ(deserialized_message.get_members().size(), 1);
    EXPECT_EQ(deserialized_message.get_members()[0], user1.get_uid());
}