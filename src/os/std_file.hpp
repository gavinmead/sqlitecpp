#ifndef SQLITECPP_STD_FILE_HPP
#define SQLITECPP_STD_FILE_HPP

#include <utility>

#include "sqlite/os/errors.hpp"
#include "sqlite/os/file.hpp"

namespace sqlite::os {

    /// Maps a POSIX errno value to a domain-specific Error.
    ///
    /// Centralises the errno-to-ErrorCode translation so every syscall site
    /// produces consistent, descriptive error messages without duplicating
    /// the same switch block.
    ///
    /// @param ec        The errno value captured immediately after the failed syscall.
    /// @param path      The file path involved, included in the error message.
    /// @param operation A short label for the syscall (e.g. "open", "pread", "fsync").
    [[nodiscard]] Error map_errno(int ec, const std::filesystem::path& path, std::string_view operation);

    /// POSIX-based implementation of the File interface.
    ///
    /// StdFile wraps a POSIX file descriptor and provides the File interface
    /// using standard system calls (pread, pwrite, fsync, ftruncate, flock).
    /// Instances are created exclusively through the open() free function.
    ///
    /// The file descriptor is owned by the StdFile and closed on destruction.
    /// Copy and move are disabled to enforce single-ownership semantics.
    class StdFile : public File {
    public:

        ~StdFile() override;
        StdFile(const StdFile&) = delete;
        StdFile& operator=(const StdFile&) = delete;
        StdFile(StdFile&&) = delete;
        StdFile& operator=(StdFile&&) = delete;

        /// Reads up to buf.size() bytes from the file starting at the given byte offset.
        /// @param buf Destination buffer to read into.
        /// @param offset Absolute byte offset in the file to begin reading.
        /// @return Number of bytes actually read, or an Error on failure.
        [[nodiscard]] auto read(std::span<std::byte> buf, uint64_t offset)
            -> std::expected<size_t, Error> override;

        /// Writes buf.size() bytes to the file starting at the given byte offset.
        /// @param buf Source buffer containing data to write.
        /// @param offset Absolute byte offset in the file to begin writing.
        /// @return Number of bytes actually written, or an Error on failure.
        [[nodiscard]] auto write(std::span<const std::byte> buf, uint64_t offset)
            -> std::expected<size_t, Error> override;

        /// Returns the current size of the file in bytes.
        [[nodiscard]] auto size() -> std::expected<uint64_t, Error> override;

        /// Flushes all pending writes to durable storage (fsync).
        [[nodiscard]] auto sync() -> std::expected<void, Error> override;

        /// Truncates or extends the file to exactly new_size bytes.
        /// @param new_size The desired file size in bytes.
        [[nodiscard]] auto truncate(uint64_t new_size) -> std::expected<void, Error> override;

        /// Acquires a file lock of the specified type.
        /// @param type The lock level to acquire (shared or exclusive).
        [[nodiscard]] auto lock(LockType type) -> std::expected<void, Error> override;

        /// Releases any file lock held by this process.
        [[nodiscard]] auto unlock() -> std::expected<void, Error> override;

        /// The open() factory function is a friend so it can construct StdFile instances.
        friend auto open(const std::filesystem::path& path, OpenOptions opts)
            -> std::expected<std::unique_ptr<File>, Error>;

    private:
        explicit StdFile(int fd, std::filesystem::path path)
            : fd_(fd), path_(std::move(path)) {};

        int fd_;
        std::filesystem::path path_;
    };

}

#endif //SQLITECPP_STD_FILE_HPP
