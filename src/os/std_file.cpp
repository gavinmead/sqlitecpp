#include "std_file.hpp"

#include <format>
#include <fcntl.h>
#include <unistd.h>

namespace sqlite::os {

    StdFile::~StdFile() {
        close(fd_);
    }

    auto open(const std::filesystem::path& path, OpenOptions opts)
          -> std::expected<std::unique_ptr<File>, Error> {

        if (!is_regular_file(path)) {
            return std::unexpected(make_error(ErrorCode::InvalidArgument, std::format("path {} is not a file", path.string())));
        }

        //Set a bunch of flags
        //if opts.create && exclusve -? create a new file, fail if it already exists
        //if opts.create && !exclusive; open an existing file, or create one if absent
        //if opts.read_only set truncate to false.

        if (opts.create && opts.exclusive) {
            //See if the path is a file and exists
        }

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
