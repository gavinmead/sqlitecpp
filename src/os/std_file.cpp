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

        int flags = O_RDWR;

        if (opts.create) {
            flags |= O_CREAT;
        }

        if (opts.exclusive) {
            flags |= O_EXCL;
        }

        if (opts.truncate) {
            flags |= O_TRUNC;
        }

        if (opts.read_only) {
            flags |= O_RDONLY;
        }

        auto fd = ::open(path.string().c_str(), flags);
        if (fd == -1) {
            auto ec = errno;

            switch (ec) {
                case ENOENT:
                    return std::unexpected(make_error(ErrorCode::FileNotFound,
                        std::format("file {} does not exist", path.string())));
                case EEXIST:
                    return std::unexpected(make_error(ErrorCode::AlreadyExists,
                        std::format("file {} already exists", path.string())));
                case EACCES:
                    return std::unexpected(make_error(ErrorCode::PermissionDenied,std::format("permission denied for file {}", path.string())));
                default:
                    return std::unexpected(make_error(ErrorCode::IoError,
                        std::format("failed to open {}: {}", path.string(), std::strerror(ec))));
            }
        }

        return std::unique_ptr<File>(new StdFile(fd, path, opts.read_only));
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
