#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <random>
#include <string>
#include <gmock/gmock-matchers.h>

#include <gtest/gtest.h>

#include "sqlite/os/file.hpp"

namespace fs = std::filesystem;
using namespace sqlite::os;
using ::testing::AllOf;
using ::testing::HasSubstr;

/// Test fixture that creates a unique temporary directory before each test
/// and removes it (along with all contents) after each test, regardless of
/// pass/fail status.
class StdFileTest : public ::testing::Test {
protected:
    fs::path tmp_dir_;

    void SetUp() override {
        auto base = fs::temp_directory_path();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dist;

        tmp_dir_ = base / std::format("sqlitecpp_test_{}", dist(gen));
        fs::create_directory(tmp_dir_);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(tmp_dir_, ec);
    }

    /// Helper: returns a path inside the fixture's temp directory.
    [[nodiscard]] fs::path tmp_path(const std::string& filename) const {
        return tmp_dir_ / filename;
    }

    /// Helper: creates a file with the given contents and returns its path.
    [[nodiscard]] fs::path create_file(const std::string& filename,
                                       const std::string& contents = "") const {
        auto p = tmp_path(filename);
        std::ofstream ofs(p, std::ios::binary);
        ofs << contents;
        return p;
    }
};

TEST_F(StdFileTest, OpenOnDirectoryFailure) {
    //open will delegate to the StdFile implementation in the CMakeList.txt
    auto result = sqlite::os::open(tmp_dir_, {});

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::InvalidArgument);
    EXPECT_THAT(result.error().message,
          AllOf(HasSubstr(tmp_dir_.string()), HasSubstr("not a file")));
}

TEST_F(StdFileTest, OpenCreateExclusiveFileExistsFailure) {
    auto file = create_file("test.db", {});

    auto result = sqlite::os::open(file, {.create=true, .exclusive = true});

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::AlreadyExists);
    EXPECT_THAT(result.error().message,
          AllOf(HasSubstr("create and exclusive options are true and file"), HasSubstr(file.string()), HasSubstr("already exists")));
}

TEST_F(StdFileTest, OpenCreateNew) {
    auto file = tmp_dir_ / "test.db";

    auto result = sqlite::os::open(file, {.create=true, .exclusive = false});

    ASSERT_TRUE(result.has_value());
    EXPECT_NE(*result, nullptr);
}

TEST_F(StdFileTest, FixtureCreatesAndCleansUpTempDir) {
    // Verify the directory exists during the test
    EXPECT_TRUE(fs::exists(tmp_dir_));
    EXPECT_TRUE(fs::is_directory(tmp_dir_));
}