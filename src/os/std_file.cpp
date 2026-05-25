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
        // close() can fail (notably on NFS), but a destructor has no way to report
        // it, so the result is intentionally discarded. fd_ is reset to guard
        // against accidental reuse if the class grows more teardown logic later.
        (void)::close(fd_);
        fd_ = -1;
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
    /// The regular-file check is performed by fstat()-ing the descriptor returned
    /// by ::open() rather than stat()-ing the path beforehand. This avoids a
    /// time-of-check/time-of-use race where the path could be swapped (e.g. for a
    /// symlink) between the check and the open.
    ///
    /// @param path Filesystem path to the file to open or create.
    /// @param opts Controls creation, exclusivity, truncation, and read-only mode.
    /// @return A unique_ptr<File> on success, or an Error describing the failure.
    auto open(const std::filesystem::path& path, OpenOptions opts)
          -> std::expected<std::unique_ptr<File>, Error> {

        // A read-only handle can never satisfy a create, so the combination is a
        // caller error rather than something to silently accept.
        if (opts.read_only && opts.create) {
            return std::unexpected(make_error(ErrorCode::InvalidArgument,
                std::format("open {}: read_only and create are mutually exclusive", path.string())));
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

        // The mode argument only applies when creating; pass it only with O_CREAT
        // so a reader isn't misled into thinking it always takes effect.
        // 0644 = owner read/write, group/other read.
        constexpr mode_t kCreateMode = 0644;
        auto fd = (flags & O_CREAT)
            ? ::open(path.string().c_str(), flags, kCreateMode)
            : ::open(path.string().c_str(), flags);
        if (fd == -1) {
            // Opening a directory with a writable mode fails with EISDIR; surface
            // it as the same "not a file" InvalidArgument as other non-regular paths.
            if (errno == EISDIR) {
                return std::unexpected(make_error(ErrorCode::InvalidArgument,
                    std::format("path {} is not a file", path.string())));
            }
            return std::unexpected(map_errno(errno, path, "open"));
        }

        struct stat statbuf;
        if (::fstat(fd, &statbuf) == -1) {
            auto err = map_errno(errno, path, "fstat"); // GCOVR_EXCL_LINE
            (void)::close(fd);                          // GCOVR_EXCL_LINE
            return std::unexpected(err);                // GCOVR_EXCL_LINE
        }

        if (!S_ISREG(statbuf.st_mode)) {
            (void)::close(fd);
            return std::unexpected(make_error(ErrorCode::InvalidArgument,
                std::format("path {} is not a file", path.string())));
        }

        return std::unique_ptr<File>(new StdFile(fd, path));
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

        // Retry on EINTR: a signal can interrupt the syscall before any bytes are
        // transferred, which is not a real error. A short read still returns here
        // (callers handle short reads per the File contract).
        ssize_t result;
        do {
            result = ::pread(fd_, buf.data(), buf.size(), offset);
        } while (result == -1 && errno == EINTR);

        if (result == -1) {
            return std::unexpected(map_errno(errno, path_, "read")); // GCOVR_EXCL_LINE
        }

        return static_cast<size_t>(result);
    }

    auto StdFile::write(std::span<const std::byte> buf, uint64_t offset)
        -> std::expected<size_t, Error> {

        if (fd_ == -1) {
            return std::unexpected(map_errno(EBADF, path_, "write")); // GCOVR_EXCL_LINE
        }

        if (buf.empty()) {
            return 0;
        }

        // Retry on EINTR (see read()). A short write is still returned to the
        // caller, who is responsible for writing the remainder.
        ssize_t result;
        do {
            result = ::pwrite(fd_, buf.data(), buf.size(), offset);
        } while (result == -1 && errno == EINTR);

        if (result == -1) {
            return std::unexpected(map_errno(errno, path_, "write"));
        }

        return static_cast<size_t>(result);
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
