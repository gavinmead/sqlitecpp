#include <gtest/gtest.h>
#include "db_header.h"

TEST(DBHeader, DefaultDBHeader) {
    sql::DefaultDBHeader header(512);
    EXPECT_EQ(header.name(), "SQLite format 3\0");
    EXPECT_EQ(header.pageSize(), 512);
};

TEST(DBHeader, DefaultDBHeaderPageSize65536) {
    sql::DefaultDBHeader header(1);
    EXPECT_EQ(header.name(), "SQLite format 3\0");
    EXPECT_EQ(header.pageSize(), 65536);
};
