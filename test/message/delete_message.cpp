#include <gtest/gtest.h>

#include "constants.hpp"
#include "message/header.hpp"
#include "message/delete_message.hpp"
#include "models/message.hpp"
#include "models/user.hpp"
#include "models/channel.hpp"

TEST(DeleteMessage, SerializesDeserializesProperly)
{
    User user("username", "display_name");
    Channel channel("channel", {user.get_uid()});
    Message message(user.get_uid(), channel.get_uid(), "text");
    DeleteMessageMessage delete_message(channel.get_uid() , message.get_snowflake());

    std::vector<uint8_t> buf;

    delete_message.serialize_msg(buf);

    Header deserialized_header;
    DeleteMessageMessage deserialized_message;
    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));

    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::DELETE_MESSAGE);
    EXPECT_EQ(deserialized_message.get_message_snowflake(), message.get_snowflake());
}