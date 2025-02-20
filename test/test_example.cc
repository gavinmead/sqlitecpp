//
// Created by Gavin Mead on 2/20/25.
//
#include <gtest/gtest.h>
#include "example.h"

TEST(ExampleTest, Addition) {
    Example example;
    EXPECT_EQ(example.add(2, 3), 5);
    EXPECT_EQ(example.add(-1, 1), 0);
    EXPECT_EQ(example.add(0, 0), 0);
}
