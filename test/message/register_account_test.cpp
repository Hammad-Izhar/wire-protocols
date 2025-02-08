#include <gtest/gtest.h>

#include "message/register_account.hpp"
#include "message/header.hpp"

TEST(RegisterAccount, ConstructsValidRegisterAccountMessage)
{
    RegisterAccountMessage message("username", "password", "display_name");

    EXPECT_EQ(message.get_username(), "username");
    EXPECT_EQ(message.get_password(), "password");
    EXPECT_EQ(message.get_display_name(), "display_name");
}

TEST(RegisterAccount, RegisterAccountSerializesProperly)
{
    RegisterAccountMessage message("username", "password", "display_name");
    std::vector<uint8_t> buf;
    message.serialize(buf);

    EXPECT_EQ(buf.size(), 3 + 8 + 8 + 12); // 3 bytes for lengths, 8 bytes for the first two strings, and 12 bytes for the last string
    EXPECT_EQ(buf[0], 8);                  // username length
    EXPECT_EQ(buf[9], 8);                  // password length
    EXPECT_EQ(buf[18], 12);                // display name length
    EXPECT_EQ(std::string(buf.begin() + 1, buf.begin() + 1 + 8), "username");
    EXPECT_EQ(std::string(buf.begin() + 10, buf.begin() + 10 + 8), "password");
    EXPECT_EQ(std::string(buf.begin() + 19, buf.begin() + 19 + 12), "display_name");
}

TEST(RegisterAccount, RegisterAccountDeserializesProperly)
{
    std::vector<uint8_t> buf = {8, 'u', 's', 'e', 'r', 'n', 'a', 'm', 'e', 8, 'p', 'a', 's', 's', 'w', 'o', 'r', 'd', 12, 'd', 'i', 's', 'p', 'l', 'a', 'y', '_', 'n', 'a', 'm', 'e'};
    RegisterAccountMessage message;
    message.deserialize(buf);

    EXPECT_EQ(message.get_username(), "username");
    EXPECT_EQ(message.get_password(), "password");
    EXPECT_EQ(message.get_display_name(), "display_name");
}

TEST(RegisterAccount, ConstructsValidCompleteMessage)
{
    RegisterAccountMessage message("username", "password", "display_name");
    std::vector<uint8_t> buf;

    message.serialize_msg(buf);

    RegisterAccountMessage deserialized_message;
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + Header::size(), buf.end()));

    EXPECT_EQ(buf.size(), Header::size() + message.size());
    EXPECT_EQ(deserialized_message.get_username(), "username");
    EXPECT_EQ(deserialized_message.get_password(), "password");
    EXPECT_EQ(deserialized_message.get_display_name(), "display_name");
}
