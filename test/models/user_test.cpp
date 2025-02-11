#include <gtest/gtest.h>
#include "models/user.hpp"
#include <stdexcept>

TEST(UserTest, MakeUser)
{
    User user("tomdavkam", "passphrase", "thomas");

    EXPECT_EQ(user.get_username(), "tomdavkam");
    EXPECT_EQ(user.get_display_name(), "thomas");
    EXPECT_EQ(user.get_profile_pic(),  "./assets/profile_pics/blank_profile_pic.png");
}

TEST(UserTest, DifferentUIDs)
{
    User user1("tomdavkam", "banana", "thomas");
    User user2("hizhar", "apple", "hammad");

    EXPECT_NE(user1.get_uid(), user2.get_uid());
}

TEST(UserTest, AddChannel)
{
    User user("tomdavkam", "banana", "thomas");
    std::array<uint8_t,16> channel1 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    std::array<uint8_t,16> channel2 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2};

    user.add_channel(channel1);

    EXPECT_EQ(user.get_channels().size(), 1);
    EXPECT_EQ(user.get_channels()[0], channel1);

    user.add_channel(channel2);

    EXPECT_EQ(user.get_channels().size(), 2);
    EXPECT_EQ(user.get_channels()[1], channel2);
}

TEST(UserTest, RemoveChannel)
{
    User user("tomdavkam", "banana", "thomas");
    std::array<uint8_t,16> channel1 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    std::array<uint8_t,16> channel2 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2};
    user.add_channel(channel1);
    user.add_channel(channel2);
    EXPECT_EQ(user.get_channels().size(), 2);
    user.remove_channel(channel1);
    EXPECT_EQ(user.get_channels().size(), 1);
    EXPECT_EQ(user.get_channels()[0], channel2);
    user.remove_channel(channel2);
    EXPECT_EQ(user.get_channels().size(), 0);
}

TEST(UserTest, RemoveNonexistentChannel)
{
    User user("tomdavkam", "banana", "thomas");
    std::array<uint8_t,16> channel1 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
    std::array<uint8_t,16> channel2 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2};
    user.add_channel(channel1);
    EXPECT_THROW(user.remove_channel(channel2), std::runtime_error);
}