#include <gtest/gtest.h>

#include "constants.hpp"
#include "message/delete_account.hpp"
#include "message/header.hpp"

TEST(DeleteAccount, ConstructsValidDeleteAccountMessage) {
    DeleteAccountMessage message("username", "password");

    EXPECT_EQ(message.get_username(), "username");
    EXPECT_EQ(message.get_password(), "password");
}

TEST(DeleteAccount, DeleteAccountSerializesProperly) {
    DeleteAccountMessage message("username", "password");
    std::vector<uint8_t> buf;
    message.serialize(buf);

    EXPECT_EQ(buf.size(), (1 + 8) + (1 + 8));  // 3 bytes for lengths, 8 bytes for the first two
                                               // strings, and 12 bytes for the last string
    EXPECT_EQ(buf[0], 8);                      // username length
    EXPECT_EQ(buf[9], 8);                      // password length
    EXPECT_EQ(std::string(buf.begin() + 1, buf.begin() + 1 + 8), "username");
    EXPECT_EQ(std::string(buf.begin() + 10, buf.begin() + 10 + 8), "password");
}

TEST(DeleteAccount, DeleteAccountDeserializesProperly) {
    std::vector<uint8_t> buf = {8, 'u', 's', 'e', 'r', 'n', 'a', 'm', 'e',
                                8, 'p', 'a', 's', 's', 'w', 'o', 'r', 'd'};
    DeleteAccountMessage message;
    message.deserialize(buf);

    EXPECT_EQ(message.get_username(), "username");
    EXPECT_EQ(message.get_password(), "password");
}

TEST(DeleteAccount, ConstructsValidCompleteMessage) {
    DeleteAccountMessage message("username", "password");
    std::vector<uint8_t> buf;

    message.serialize_msg(buf);

    Header deserialized_header;
    DeleteAccountMessage deserialized_message;
    deserialized_header.deserialize(
        std::vector<uint8_t>(buf.begin(), buf.begin() + deserialized_header.size()));
    deserialized_message.deserialize(std::vector<uint8_t>(buf.begin() + deserialized_header.size(), buf.end()));

    EXPECT_EQ(buf.size(), deserialized_header.size() + message.size());
    EXPECT_EQ(deserialized_header.get_version(), PROTOCOL_VERSION);
    EXPECT_EQ(deserialized_header.get_operation(), Operation::DELETE_ACCOUNT);
    EXPECT_EQ(deserialized_header.get_packet_length(), message.size());
    EXPECT_EQ(deserialized_message.get_username(), "username");
    EXPECT_EQ(deserialized_message.get_password(), "password");
}
