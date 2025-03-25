#include <gtest/gtest.h>
#include "server/db/password_table.hpp"
#include "models/uuid.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

// Helper function to split a string by a delimiter.
static std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

// Fixture for persistent PasswordTable tests.
class PasswordTablePersistentTestFixture : public ::testing::Test {
protected:
    std::string test_dir = "test_dir";

    void SetUp() override {
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
        std::filesystem::create_directory(test_dir);
    }

    void TearDown() override {
        if (std::filesystem::exists(test_dir)) {
            std::filesystem::remove_all(test_dir);
        }
    }
};

TEST_F(PasswordTablePersistentTestFixture, SaveAndLoadPassword) {
    UUID user;
    {
        // Create a PasswordTable and add a password.
        PasswordTable pt(test_dir);
        auto result = pt.add_password(user, "password123");
        ASSERT_TRUE(std::holds_alternative<std::monostate>(result));
    }
    {
        // Reload a new PasswordTable from persistent storage.
        PasswordTable reloaded(test_dir);
        const auto& data = reloaded.get_data();
        auto it = data.find(user);
        ASSERT_TRUE(it != data.end());
        // Verify that the correct password returns true.
        auto verify = reloaded.verify_password(user, "password123");
        ASSERT_TRUE(std::holds_alternative<bool>(verify));
        EXPECT_TRUE(std::get<bool>(verify));
    }
}

TEST_F(PasswordTablePersistentTestFixture, PersistentDataMatchesCSV) {
    UUID user;
    {
        // Create a PasswordTable and add a password.
        PasswordTable pt(test_dir);
        pt.add_password(user, "password123");
    }
    // Reload the table.
    PasswordTable reloaded(test_dir);
    const auto& data = reloaded.get_data();
    // Expect exactly one entry.
    ASSERT_EQ(data.size(), 1);

    // Open the CSV file directly.
    std::ifstream file(test_dir + "/passwords.csv");
    ASSERT_TRUE(file.is_open());
    std::string header;
    std::getline(file, header);  // Skip header line.
    std::string line;
    ASSERT_TRUE(std::getline(file, line));
    auto tokens = split(line, '|');  // Assuming '|' is used as the delimiter.
    // We expect 3 tokens: uid, hashed_password, and salt.
    EXPECT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0], user.to_string());
    // Optionally, check that the hashed password and salt are not empty.
    EXPECT_FALSE(tokens[1].empty());
    EXPECT_FALSE(tokens[2].empty());
}
