#include <gtest/gtest.h>

// Placeholder test to verify the test infrastructure works
TEST(Placeholder, BuildSystemWorks) {
    EXPECT_TRUE(true);
}

TEST(Placeholder, Cpp23Features) {
    // Test that C++23 features are available
    auto lambda = []<typename T>(T value) { return value; };
    EXPECT_EQ(lambda(42), 42);
}
