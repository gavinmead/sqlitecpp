#include "std_file.hpp"

#include <cerrno>
#include <cstring>
#include <memory>
#include <format>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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
            // GCOVR_EXCL_START -- can't reliably produce a full disk, I/O fault,
            // or bad-address fault from a unit test
            case ENOSPC:
                return make_error(ErrorCode::IoError,
                    std::format("{} failed: no space left on device for {}", operation, path.string()));
            case EIO:
                return make_error(ErrorCode::IoError,
                    std::format("{} failed: hardware/driver I/O failure for {}", operation, path.string()));
            case EFAULT:
                return make_error(ErrorCode::InvalidArgument,
                    std::format("{} failed: buffer pointer outside accessible address space for {}", operation, path.string()));
            // GCOVR_EXCL_STOP
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

        // O_RDONLY/O_RDWR are mutually exclusive access modes, not OR-able flags
        // (O_RDONLY is 0), so the access mode must be chosen, not accumulated.
        int flags = opts.read_only ? O_RDONLY : O_RDWR;

        if (opts.create) {
            flags |= O_CREAT;
        }

        if (opts.exclusive) {
            flags |= O_EXCL;
        }

        if (opts.truncate) {
            flags |= O_TRUNC;
        }

        // When O_CREAT is set, open() reads a mode argument; omitting it leaves
        // the new file's permissions undefined. 0644 = owner read/write, group/other read.
        constexpr mode_t kCreateMode = 0644;
        auto fd = ::open(path.string().c_str(), flags, kCreateMode);
        if (fd == -1) {
            return std::unexpected(map_errno(errno, path, "open"));
        }

        return std::unique_ptr<File>(new StdFile(fd, path, opts.read_only));
    }

    auto StdFile::read(std::span<std::byte> buf, uint64_t offset)
        -> std::expected<size_t, Error> {

        if (fd_ == -1) {
            return std::unexpected(map_errno(EBADF, path_, "pread")); // GCOVR_EXCL_LINE
        }

        // No reason for a syscall here
        if (buf.empty()) {
            return 0;
        }

        auto result = ::pread(fd_, buf.data(), buf.size(), offset);

        if (result == -1) {
            return std::unexpected(map_errno(errno, path_, "read")); // GCOVR_EXCL_LINE
        }

        return result;
    }

    auto StdFile::write(std::span<const std::byte> buf, uint64_t offset)
        -> std::expected<size_t, Error> {

        if (fd_ == -1) {
            return std::unexpected(map_errno(EBADF, path_, "write")); // GCOVR_EXCL_LINE
        }

        if (buf.empty()) {
            return 0;
        }

        auto result = ::pwrite(fd_, buf.data(), buf.size(), offset);

        if (result == -1) {
            return std::unexpected(map_errno(errno, path_, "write"));
        }

        return result;
    }

    auto StdFile::size() -> std::expected<uint64_t, Error> {
        if (fd_ == -1) {
            return std::unexpected(map_errno(EBADF, path_, "size")); // GCOVR_EXCL_LINE
        }

        struct stat statbuf;
        if (fstat(fd_, &statbuf) == -1) {
            return std::unexpected(map_errno(errno, path_, "fstat")); // GCOVR_EXCL_LINE
        }

        return statbuf.st_size;
    }

    auto StdFile::sync() -> std::expected<void, Error> {
        if (fd_ == -1) {
            return std::unexpected(map_errno(EBADF, path_, "sync")); // GCOVR_EXCL_LINE
        }

        auto result = ::fsync(fd_);
        if (result == -1) {
            return std::unexpected(map_errno(errno, path_, "sync")); // GCOVR_EXCL_LINE
        }
        return {};
    }

    auto StdFile::truncate(uint64_t new_size) -> std::expected<void, Error> {
        if (fd_ == -1) {
            return std::unexpected(map_errno(EBADF, path_, "truncate")); // GCOVR_EXCL_LINE
        }

        auto result = ::ftruncate(fd_, new_size);
        if (result == -1) {
            return std::unexpected(map_errno(errno, path_, "truncate"));
        }

        return {};
    }

    auto StdFile::lock(LockType type) -> std::expected<void, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

    auto StdFile::unlock() -> std::expected<void, Error> {
        return std::unexpected(make_error(ErrorCode::NotSupported, "not implemented"));
    }

}
