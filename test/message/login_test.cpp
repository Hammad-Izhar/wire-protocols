#include <gtest/gtest.h>

#include "message/login.hpp"
#include "message/header.hpp"
#include "constants.hpp"

TEST(Login, ConstructsValidLoginMessage)
{
    LoginMessage message("username", "password");

    EXPECT_EQ(message.get_username(), "username");
    EXPECT_EQ(message.get_password(), "password");
}

TEST(Login, LoginSerializesProperly)
{
    LoginMessage message("username", "password");
    std::vector<uint8_t> buf;
    message.serialize(buf);

    EXPECT_EQ(buf.size(), (1 + 8) + (1 + 8)); // 3 bytes for lengths, 8 bytes for the first two strings, and 12 bytes for the last string
    EXPECT_EQ(buf[0], 8);                     // username length
    EXPECT_EQ(buf[9], 8);                     // password length
    EXPECT_EQ(std::string(buf.begin() + 1, buf.begin() + 1 + 8), "username");
    EXPECT_EQ(std::string(buf.begin() + 10, buf.begin() + 10 + 8), "password");
}

TEST(Login, LoginDeserializesProperly)
{
    std::vector<uint8_t> buf = {8, 'u', 's', 'e', 'r', 'n', 'a', 'm', 'e', 8, 'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
    LoginMessage message;
    message.deserialize(buf);

    EXPECT_EQ(message.get_username(), "username");
    EXPECT_EQ(message.get_password(), "password");
}

TEST(Login, ConstructsValidCompleteMessage)
{
    LoginMessage message("username", "password");
    std::vector<uint8_t> buf;

    message.serialize_msg(buf);

    Header deserialized_header;
    LoginMessage deserialized_message;
    deserialized_header.deserialize(std::vector<uint8_t>(buf.begin(), buf.begin() + Header::size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + Header::size(), buf.end()));

    EXPECT_EQ(buf.size(), Header::size() + message.size());
    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::LOGIN);
    EXPECT_EQ(deserialized_header.get_packet_length(), message.size());
    EXPECT_EQ(deserialized_message.get_username(), "username");
    EXPECT_EQ(deserialized_message.get_password(), "password");
}
