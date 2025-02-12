#include <gtest/gtest.h>

#include "constants.hpp"
#include "message/header.hpp"

TEST(HeaderTest, ConstructsValidHeader) {
    Header header(1, Operation::REGISTER_ACCOUNT, 10);

    EXPECT_EQ(header.get_version(), 1);
    EXPECT_EQ(header.get_operation(), Operation::REGISTER_ACCOUNT);
    EXPECT_EQ(header.get_packet_length(), 10);
}

TEST(HeaderTest, HeaderSerializesProperly) {
    Header header(1, Operation::REGISTER_ACCOUNT, 10);
    std::vector<uint8_t> buf;
    header.serialize(buf);

    EXPECT_EQ(buf.size(),
              4);  // 1 byte for version, 1 byte for operation, 2 bytes for packet length
    EXPECT_EQ(
        buf[0],
        (1 << 4) | 4);  // the upper 4 bits are the version, the lower 4 bits are 4, the header size
    EXPECT_EQ(buf[1], 0x00);  // REGISTER_ACCOUNT is 0
    EXPECT_EQ(buf[2], 0x00);  // Packet length is a u16, so its bytes are 0x000A
    EXPECT_EQ(buf[3],
              0x0A);  // Since NBO is big endian, the first byte is 0x00 and the second byte is 0x0A
}

TEST(HeaderTest, HeaderDeserializesProperly) {
    std::vector<uint8_t> buf = {(1 << 4) | 4, 0x06, 0x00, 0x0A};
    Header header;
    header.deserialize(buf);

    EXPECT_EQ(header.get_version(), 1);
    EXPECT_EQ(header.get_operation(), Operation::DELETE_MESSAGE);
    EXPECT_EQ(header.get_packet_length(), 10);
}