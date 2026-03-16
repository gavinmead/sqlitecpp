#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sqlite/os/errors.hpp"

using namespace sqlite::os;

namespace sqlite::os::test {
    TEST(OSErrors, MakeErrorOk) {
        auto result = make_error(ErrorCode::IoError, "test");
        ASSERT_EQ(result.code, ErrorCode::IoError);
        ASSERT_EQ(result.message, "test");

        // source_location behaviour varies across compilers (Apple Clang in particular
        // does not reliably capture call-site file/function names for default arguments).
        // We assert only the portable guarantees: the location is non-empty and has a
        // positive line number.
        EXPECT_THAT(result.location.file_name(), testing::Not(testing::IsEmpty()));
        EXPECT_GT(result.location.line(), 0u);

    }
}