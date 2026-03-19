#include "std_file.hpp"

#include <memory>
#include <format>
#include <fcntl.h>
#include <unistd.h>

namespace sqlite::os {

    StdFile::~StdFile() {
        close(fd_);
    }

    auto open(const std::filesystem::path& path, OpenOptions opts)
          -> std::expected<std::unique_ptr<File>, Error> {

        auto file_exists = exists(path);

        if (file_exists && !is_regular_file(path)) {
            return std::unexpected(make_error(ErrorCode::InvalidArgument, std::format("path {} is not a file", path.string())));
        }

        if (opts.create && opts.exclusive) {
            if (file_exists) {
                return std::unexpected(make_error(ErrorCode::AlreadyExists, std::format("create and exclusive options are true and file {} already exists", path.string())));
            }
        }

        if (opts.create && !opts.exclusive) {
            auto fd = ::open(path.c_str(), O_CREAT);
            if (fd == -1) {
                std::unexpected(make_error(ErrorCode::IoError, std::format("failed to open file {}", path.string())));
            }

            return std::unique_ptr<File>(new StdFile(fd, path, opts.read_only));
        }

        //if opts.create && exclusve -? create a new file, fail if it already exists
        //if opts.create && !exclusive; open an existing file, or create one if absent
        //if opts.read_only set truncate to false.

        // Can access StdFile's private members here

    }

    auto StdFile::read(std::span<std::byte> buf, uint64_t offset)
        -> std::expected<size_t, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

    auto StdFile::write(std::span<const std::byte> buf, uint64_t offset)
        -> std::expected<size_t, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

    auto StdFile::size() -> std::expected<uint64_t, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

    auto StdFile::sync() -> std::expected<void, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

    auto StdFile::truncate(uint64_t new_size) -> std::expected<void, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

    auto StdFile::lock(LockType type) -> std::expected<void, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

    auto StdFile::unlock() -> std::expected<void, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

}
