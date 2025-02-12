#include <gtest/gtest.h>
#include <stdexcept>
#include <unordered_map>

#include "models/uuid.hpp"

TEST(UUIDTest, DefaultConstructor)
{
    UUID uuid;
    EXPECT_EQ(uuid.value.size(), 16);
}

TEST(UUIDTest, EqualityOperator)
{
    UUID uuid1;
    UUID uuid2;
    // 1 should be equal to itself, and not equal to 2
    EXPECT_TRUE(uuid1 == uuid1);
    EXPECT_FALSE(uuid1 == uuid2);
}

TEST(UUIDTest, IsHashable)
{
    // Test whether UUID can be used as a key in an unordered_map
    std::unordered_map<UUID, int> uuid_map;
}
