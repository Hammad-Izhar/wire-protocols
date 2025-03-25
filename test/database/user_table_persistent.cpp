#include <gtest/gtest.h>
#include "server/db/user_table.hpp"
#include "models/uuid.hpp"
#include "models/user.hpp"
#include <filesystem>

// Fixture for persistent UserTable tests.
class UserTablePersistentTestFixture : public ::testing::Test {
protected:
    std::string test_dir = "test_dir";

    void SetUp() override {
        // Remove any existing test directory and create a fresh one.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
        std::filesystem::create_directory(test_dir);
    }

    void TearDown() override {
        // Clean up the test directory.
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(UserTablePersistentTestFixture, SaveAndLoadUser) {
    UUID uid;
    {
        // Create a UserTable and add a user.
        UserTable userTable(test_dir);
        User::SharedPtr user = std::make_shared<User>("thomask", "Thomas");
        userTable.add_user(user);
        uid = user->get_uid(); // Capture the UID here.
    }
    {
        // Reload a new UserTable from persistent storage.
        UserTable reloadedTable(test_dir);
        auto user_opt = reloadedTable.get_by_uid(uid);
        EXPECT_TRUE(user_opt.has_value());
        EXPECT_EQ(user_opt.value()->get_username(), "thomask");
    }
}


TEST_F(UserTablePersistentTestFixture, RemoveUserPersistent) {
    UUID uid;
    {
        // Create a UserTable, add a user, then remove it.
        UserTable userTable(test_dir);
        User::SharedPtr user = std::make_shared<User>("thomask", "Thomas");
        userTable.add_user(user);
        uid = user->get_uid();
        userTable.remove_user(uid);
    }
    {
        // Reload a new UserTable from persistent storage.
        UserTable reloadedTable(test_dir);
        auto user_opt = reloadedTable.get_by_uid(uid);
        EXPECT_FALSE(user_opt.has_value());
    }
}
