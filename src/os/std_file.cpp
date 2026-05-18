#include "std_file.hpp"

#include <cerrno>
#include <cstring>
#include <memory>
#include <format>
#include <fcntl.h>
#include <unistd.h>

namespace sqlite::os {

    Error map_errno(int ec, const std::filesystem::path& path, std::string_view operation) {
        switch (ec) {
            case ENOENT:
                return make_error(ErrorCode::FileNotFound,
                    std::format("{} failed: {} does not exist", operation, path.string()));
            case EEXIST:
                return make_error(ErrorCode::AlreadyExists,
                    std::format("{} failed: {} already exists", operation, path.string()));
            case EACCES:
            case EAGAIN:
                return make_error(ErrorCode::PermissionDenied,
                    std::format("{} failed: permission denied for {}", operation, path.string()));
            case EBADF:
                return make_error(ErrorCode::IoError,
                    std::format("{} failed: invalid file descriptor for {}", operation, path.string()));
            case ENOSPC:
                return make_error(ErrorCode::IoError,
                    std::format("{} failed: no space left on device for {}", operation, path.string()));
            case EIO:
                return make_error(ErrorCode::IoError,
                    std::format("{} failed: hardware/driver I/O failure for {}", operation, path.string()));
            case EFAULT:
                return make_error(ErrorCode::InvalidArgument,
                    std::format("{} failed: buffer pointer outside accessible address space for {}", operation, path.string()));
            default:
                return make_error(ErrorCode::IoError,
                    std::format("{} failed on {}: {}", operation, path.string(), std::strerror(ec)));
        }
    }

    StdFile::~StdFile() {
        close(fd_);
    }

    /// Opens a file at the given path and returns a File handle.
    ///
    /// Translates OpenOptions into POSIX open flags (O_CREAT, O_EXCL, O_TRUNC, O_RDONLY)
    /// and maps errno values from the syscall to domain-specific ErrorCodes:
    ///   - ENOENT -> FileNotFound
    ///   - EEXIST -> AlreadyExists
    ///   - EACCES -> PermissionDenied
    ///   - other  -> IoError
    ///
    /// @param path Filesystem path to the file to open or create.
    /// @param opts Controls creation, exclusivity, truncation, and read-only mode.
    /// @return A unique_ptr<File> on success, or an Error describing the failure.
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
            return std::unexpected(map_errno(errno, path, "open"));
        }

        return std::unique_ptr<File>(new StdFile(fd, path, opts.read_only));
    }

    auto StdFile::read(std::span<std::byte> buf, uint64_t offset)
        -> std::expected<size_t, Error> {

        if (fd_ == -1) {
            return std::unexpected(map_errno(EBADF, path_, "pread"));
        }

        // No reason for a syscall here
        if (buf.empty()) {
            return 0;
        }

        auto result = ::pread(fd_, buf.data(), buf.size(), offset);

        if (result == -1) {
            return std::unexpected(map_errno(errno, path_, "read"));
        }

        return result;
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
