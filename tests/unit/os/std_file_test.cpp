#include <array>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <random>
#include <string>
#include <unistd.h>
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
          AllOf(HasSubstr("open failed"), HasSubstr(file.string()), HasSubstr("already exists")));
}

TEST_F(StdFileTest, OpenCreateNew) {
    auto file = tmp_dir_ / "test.db";

    auto result = sqlite::os::open(file, {.create=true, .exclusive = false});

    ASSERT_TRUE(result.has_value());
    EXPECT_NE(*result, nullptr);
}

TEST_F(StdFileTest, ReadSqliteHeaderEmptyBuffer) {
    auto db_path = fs::path(TEST_FIXTURES_DIR) / "test.db";

    auto file = sqlite::os::open(db_path, {.read_only = true});
    ASSERT_TRUE(file.has_value());

    // SQLite header is always "SQLite format 3\0" (16 bytes)
    std::array<std::byte, 0> buf{};
    auto result = (*file)->read(buf, 0);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result, 0);
}

TEST_F(StdFileTest, ReadSqliteHeader) {
    auto db_path = fs::path(TEST_FIXTURES_DIR) / "test.db";

    auto file = sqlite::os::open(db_path, {.read_only = true});
    ASSERT_TRUE(file.has_value());

    // SQLite header is always "SQLite format 3\0" (16 bytes)
    std::array<std::byte, 16> buf{};
    auto result = file.value()->read(buf, 0);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 16u);

    std::string_view header(reinterpret_cast<const char*>(buf.data()), 15);
    EXPECT_EQ(header, "SQLite format 3");
}

TEST_F(StdFileTest, FixtureCreatesAndCleansUpTempDir) {
    // Verify the directory exists during the test
    EXPECT_TRUE(fs::exists(tmp_dir_));
    EXPECT_TRUE(fs::is_directory(tmp_dir_));
}

/// Helper: views a string's bytes as a span suitable for File::write.
static std::span<const std::byte> as_bytes(std::string_view s) {
    return {reinterpret_cast<const std::byte*>(s.data()), s.size()};
}

/// Helper: reads the entire file at path into a string (independent of File).
static std::string read_all(const fs::path& path) {
    std::ifstream ifs(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>()};
}

TEST_F(StdFileTest, WriteReturnsByteCountAndPersists) {
    auto path = tmp_path("write.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    constexpr std::string_view payload = "hello world";
    auto result = (*file)->write(as_bytes(payload), 0);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, payload.size());
    EXPECT_EQ(read_all(path), payload);
}

TEST_F(StdFileTest, WriteEmptyBufferReturnsZeroAndWritesNothing) {
    auto path = tmp_path("empty.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    std::array<std::byte, 0> buf{};
    auto result = (*file)->write(buf, 0);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0u);
    EXPECT_EQ(fs::file_size(path), 0u);
}

TEST_F(StdFileTest, WriteAtOffsetCreatesSparseGap) {
    auto path = tmp_path("offset.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    constexpr std::string_view payload = "abc";
    auto result = (*file)->write(as_bytes(payload), 4);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, payload.size());

    // Bytes before the offset are zero-filled; the payload follows.
    EXPECT_EQ(read_all(path), std::string("\0\0\0\0", 4) + std::string(payload));
}

TEST_F(StdFileTest, WriteOverwritesExistingBytesAtOffset) {
    auto path = create_file("overwrite.db", "AAAAAAAA");

    auto file = sqlite::os::open(path, {});
    ASSERT_TRUE(file.has_value());

    constexpr std::string_view payload = "XY";
    auto result = (*file)->write(as_bytes(payload), 2);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, payload.size());
    EXPECT_EQ(read_all(path), "AAXYAAAA");
}

TEST_F(StdFileTest, WriteRoundTripsThroughRead) {
    auto path = tmp_path("roundtrip.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    constexpr std::string_view payload = "round trip data";
    ASSERT_TRUE((*file)->write(as_bytes(payload), 0).has_value());

    std::array<std::byte, payload.size()> buf{};
    auto read_result = (*file)->read(buf, 0);

    ASSERT_TRUE(read_result.has_value());
    EXPECT_EQ(*read_result, payload.size());
    EXPECT_EQ(std::string_view(reinterpret_cast<const char*>(buf.data()), buf.size()), payload);
}

TEST_F(StdFileTest, SizeOfEmptyFileIsZero) {
    auto path = tmp_path("empty_size.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    auto result = (*file)->size();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, 0u);
}

TEST_F(StdFileTest, SizeReflectsExistingContent) {
    constexpr std::string_view contents = "twelve bytes";
    auto path = create_file("sized.db", std::string(contents));

    auto file = sqlite::os::open(path, {});
    ASSERT_TRUE(file.has_value());

    auto result = (*file)->size();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, contents.size());
}

TEST_F(StdFileTest, SizeGrowsAfterWrite) {
    auto path = tmp_path("grow.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    constexpr std::string_view payload = "payload";
    ASSERT_TRUE((*file)->write(as_bytes(payload), 0).has_value());

    auto result = (*file)->size();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, payload.size());
}

TEST_F(StdFileTest, SizeIncludesSparseGapFromOffsetWrite) {
    auto path = tmp_path("sparse_size.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    constexpr std::string_view payload = "abc";
    constexpr uint64_t offset = 4;
    ASSERT_TRUE((*file)->write(as_bytes(payload), offset).has_value());

    auto result = (*file)->size();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, offset + payload.size());
}

TEST_F(StdFileTest, SyncSucceedsOnOpenFile) {
    auto path = tmp_path("sync.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    auto result = (*file)->sync();

    EXPECT_TRUE(result.has_value());
}

TEST_F(StdFileTest, SyncSucceedsAfterWrite) {
    auto path = tmp_path("sync_write.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    constexpr std::string_view payload = "durable bytes";
    ASSERT_TRUE((*file)->write(as_bytes(payload), 0).has_value());

    auto result = (*file)->sync();

    EXPECT_TRUE(result.has_value());
    // Data remains readable after the flush.
    EXPECT_EQ(read_all(path), payload);
}

TEST_F(StdFileTest, SyncIsIdempotent) {
    auto path = tmp_path("sync_twice.db");

    auto file = sqlite::os::open(path, {.create = true});
    ASSERT_TRUE(file.has_value());

    ASSERT_TRUE((*file)->write(as_bytes("data"), 0).has_value());

    EXPECT_TRUE((*file)->sync().has_value());
    EXPECT_TRUE((*file)->sync().has_value());
}

TEST_F(StdFileTest, TruncateShrinksFile) {
    auto path = create_file("shrink.db", "0123456789");

    auto file = sqlite::os::open(path, {});
    ASSERT_TRUE(file.has_value());

    ASSERT_TRUE((*file)->truncate(4).has_value());

    EXPECT_EQ(*(*file)->size(), 4u);
    EXPECT_EQ(read_all(path), "0123");
}

TEST_F(StdFileTest, TruncateExtendsFileWithZeros) {
    auto path = create_file("extend.db", "abc");

    auto file = sqlite::os::open(path, {});
    ASSERT_TRUE(file.has_value());

    ASSERT_TRUE((*file)->truncate(6).has_value());

    EXPECT_EQ(*(*file)->size(), 6u);
    // The original content is preserved; the new region is zero-filled.
    EXPECT_EQ(read_all(path), std::string("abc") + std::string("\0\0\0", 3));
}

TEST_F(StdFileTest, TruncateToZeroEmptiesFile) {
    auto path = create_file("emptyme.db", "not empty");

    auto file = sqlite::os::open(path, {});
    ASSERT_TRUE(file.has_value());

    ASSERT_TRUE((*file)->truncate(0).has_value());

    EXPECT_EQ(*(*file)->size(), 0u);
    EXPECT_EQ(read_all(path), "");
}

TEST_F(StdFileTest, TruncateToSameSizeIsNoOp) {
    constexpr std::string_view contents = "unchanged";
    auto path = create_file("same.db", std::string(contents));

    auto file = sqlite::os::open(path, {});
    ASSERT_TRUE(file.has_value());

    ASSERT_TRUE((*file)->truncate(contents.size()).has_value());

    EXPECT_EQ(*(*file)->size(), contents.size());
    EXPECT_EQ(read_all(path), contents);
}

TEST_F(StdFileTest, OpenNonExistentWithoutCreateFails) {
    auto path = tmp_path("missing.db");

    auto result = sqlite::os::open(path, {});

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::FileNotFound);
}

TEST_F(StdFileTest, OpenWithoutPermissionFails) {
    if (::geteuid() == 0) {
        GTEST_SKIP() << "running as root bypasses permission checks";
    }

    auto path = create_file("noperm.db", "data");
    fs::permissions(path, fs::perms::none);

    auto result = sqlite::os::open(path, {});

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::PermissionDenied);
}

TEST_F(StdFileTest, WriteToReadOnlyFileFails) {
    auto path = create_file("readonly.db", "existing");

    auto file = sqlite::os::open(path, {.read_only = true});
    ASSERT_TRUE(file.has_value());

    auto result = (*file)->write(as_bytes("nope"), 0);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::IoError);
}

TEST_F(StdFileTest, TruncateReadOnlyFileFails) {
    auto path = create_file("readonly_trunc.db", "existing");

    auto file = sqlite::os::open(path, {.read_only = true});
    ASSERT_TRUE(file.has_value());

    auto result = (*file)->truncate(2);

    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, ErrorCode::IoError);
}