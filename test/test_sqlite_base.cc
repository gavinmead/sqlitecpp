//
// Created by Gavin Mead on 2/21/25.
//

#include <gtest/gtest.h>
#include "sqlite.h"
#include <filesystem>
#include <string>
#include <random>
#include <algorithm>
#include <memory>

namespace fs = std::filesystem;
using namespace sql;

std::string generate_random_string(size_t length) {
    const std::basic_string characters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string random_string;
    for (size_t i = 0; i < length; ++i) {
        random_string += characters[distribution(generator)];
    }

    return random_string;
}

namespace {

    class FileSetupFixture : public testing::Test {
    protected:
        void SetUp() override {
            this->dir = fs::temp_directory_path().append(generate_random_string(10));
            fs::create_directory(this->dir);
            this->filename = dir.string() + generate_random_string(10) + ".db3";
        }

        void TearDown() override {
            //Delete the temp directory
            fs::remove_all(this->dir);
        }

        fs::path dir;
        std::string filename;

    };

    TEST_F(FileSetupFixture, DefaultConstructor) {
        EXPECT_NO_THROW({auto conn = SQLiteConnection(filename);});

    };

    TEST_F(FileSetupFixture, CreateOpenAndClose) {
        auto conn = std::make_unique<SQLiteConnection>(SQLiteConnection(filename));
        auto result = conn->open();
        ASSERT_EQ(result, SQLITE_OK);

        if (auto last_message_opt = conn->lastMessage()) {
            const auto& last_message = last_message_opt.value();
            ASSERT_EQ(last_message.result, SQLITE_OK);
            ASSERT_FALSE(last_message.isError);
            ASSERT_EQ(last_message.message, "new database created successfully");
        } else {
            FAIL();
        }

        result = conn->close();
        ASSERT_EQ(result, SQLITE_OK);
        if (auto last_message_opt = conn->lastMessage()) {
            const auto& last_message = last_message_opt.value();
            ASSERT_EQ(last_message.result, SQLITE_OK);
            ASSERT_FALSE(last_message.isError);
            ASSERT_EQ(last_message.message, "database closed");
        } else {
            FAIL();
        }
    };

    TEST(SQLiteConnection, NoLastMessage) {
        auto conn = std::make_unique<SQLiteConnection>(SQLiteConnection("test"));
        ASSERT_FALSE(conn->lastMessage().has_value());
    };
};
