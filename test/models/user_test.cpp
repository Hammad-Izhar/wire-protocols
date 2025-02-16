#include <gtest/gtest.h>
#include <stdexcept>
#include <iostream>

#include "models/user.hpp"
TEST(UserTest, MakeUser) {
    User user("tomdavkam", "thomas");

    EXPECT_EQ(user.get_username(), "tomdavkam");
    EXPECT_EQ(user.get_display_name(), "thomas");
    EXPECT_EQ(user.get_profile_pic(), ":/assets/profile_pics/blank_profile_pic.png");
}

TEST(UserTest, DifferentUIDs) {
    User user1("tomdavkam", "thomas");
    User user2("hizhar", "hammad");

    EXPECT_NE(user1.get_uid(), user2.get_uid());
}

TEST(UserTest, AddChannel) {
    User user("tomdavkam", "thomas");
    UUID channel1 = UUID();
    UUID channel2 = UUID();

    user.add_channel(channel1);

    EXPECT_EQ(user.get_channels().size(), 1);
    EXPECT_EQ(user.get_channels()[0], channel1);

    user.add_channel(channel2);

    EXPECT_EQ(user.get_channels().size(), 2);
    EXPECT_EQ(user.get_channels()[1], channel2);
}

TEST(UserTest, RemoveChannel) {
    User user("tomdavkam", "thomas");
    UUID channel1 = UUID();
    UUID channel2 = UUID();
    user.add_channel(channel1);
    user.add_channel(channel2);
    EXPECT_EQ(user.get_channels().size(), 2);
    user.remove_channel(channel1);
    EXPECT_EQ(user.get_channels().size(), 1);
    EXPECT_EQ(user.get_channels()[0], channel2);
    user.remove_channel(channel2);
    EXPECT_EQ(user.get_channels().size(), 0);
}

TEST(UserTest, RemoveNonexistentChannel) {
    User user("tomdavkam", "thomas");
    UUID channel1 = UUID();
    UUID channel2 = UUID();

    user.add_channel(channel1);

    EXPECT_EQ(user.get_channels().size(), 1);
    user.remove_channel(channel2);
    EXPECT_EQ(user.get_channels().size(), 1);
}

TEST(UserTest, Serialize) {
    User user("tomdavkam", "thomas");
    std::vector<uint8_t> buf;
    user.serialize(buf);

    User deserialized_user;
    deserialized_user.deserialize(buf);

    EXPECT_EQ(user.get_uid(), deserialized_user.get_uid());
}