//
// Created by Gavin Mead on 2/21/25.
//

#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <random>
#include <string>
#include "../file_fixture.h"
#include "sqlite.h"

namespace fs = std::filesystem;
using namespace sql;

TEST_F(FileSetupFixture, DefaultConstructor) {
  auto conn = std::make_unique<SQLiteConnection>(SQLiteConnection(filename));
  auto header = conn->header();
  // Header is only available after the database is opened
  ASSERT_FALSE(header.has_value());
};

TEST_F(FileSetupFixture, HeaderAvailableAfterOpen) {
  auto conn = std::make_unique<SQLiteConnection>(SQLiteConnection(filename));
  auto result = conn->open();
  auto header = conn->header();
  ASSERT_TRUE(header.has_value());
};

TEST_F(FileSetupFixture, HeaderNotAvailableAfterClose) {
  auto conn = std::make_unique<SQLiteConnection>(SQLiteConnection(filename));
  auto result = conn->open();
  result = conn->close();
  ASSERT_EQ(result, SQLITE_OK);
  auto header = conn->header();
  // Header is only available after the database is opened
  ASSERT_FALSE(header.has_value());
}

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

