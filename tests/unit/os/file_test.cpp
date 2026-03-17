#include <expected>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sqlite/os/errors.hpp"
#include "fixtures/mock_file.hpp"


using ::testing::Return;
using ::testing::_;
using namespace sqlite::os;

TEST(File, ReadReturnsExpectedByteCount) {
    MockFile file;
    std::array<std::byte, 64> buf{};

    EXPECT_CALL(file, read(_, 0))
        .WillOnce(Return(std::expected<size_t, Error>{64}));

    auto result = file.read(buf, 0);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 64);
}
