#ifndef SQLITECPP_FILE_HPP
#define SQLITECPP_FILE_HPP

#include <expected>
#include <filesystem>
#include <span>
#include <memory>

#include "errors.hpp"

namespace sqlite::os {
    enum class LockType;

    /// Abstract interface for file I/O operations used by the pager layer.
    ///
    /// All operations are non-throwing; errors are returned via std::expected.
    /// Callers must check return values — all methods are marked [[nodiscard]].
    ///
    /// Locking follows SQLite's locking model:
    ///   Shared -> Reserved -> Exclusive
    /// Multiple readers may hold Shared locks simultaneously. Reserved signals
    /// intent to write. Exclusive is required before modifying file contents.
    class File {
    public:
        virtual ~File() = default;

        /// Reads up to buf.size() bytes from the file starting at offset.
        ///
        /// @param buf    Buffer to read into. At most buf.size() bytes are read.
        /// @param offset Byte offset from the start of the file.
        /// @returns      Number of bytes actually read, or an Error on failure.
        ///               A short read (result < buf.size()) indicates EOF.
        [[nodiscard]] virtual auto read(std::span<std::byte> buf, uint64_t offset)
            -> std::expected<size_t, Error> = 0;

        /// Writes buf.size() bytes to the file starting at offset.
        ///
        /// @param buf    Data to write. Extends the file if offset + buf.size() > size().
        /// @param offset Byte offset from the start of the file.
        /// @returns      Number of bytes actually written, or an Error on failure.
        [[nodiscard]] virtual auto write(std::span<const std::byte> buf, uint64_t offset)
            -> std::expected<size_t, Error> = 0;

        /// Returns the current size of the file in bytes.
        [[nodiscard]] virtual auto size() -> std::expected<uint64_t, Error> = 0;

        /// Flushes any buffered writes to durable storage.
        ///
        /// The pager calls this after writing the journal and again after writing
        /// data pages to ensure crash-recovery guarantees are upheld.
        [[nodiscard]] virtual auto sync() -> std::expected<void, Error> = 0;

        /// Truncates or extends the file to exactly new_size bytes.
        ///
        /// If new_size is smaller than the current size, data beyond new_size is
        /// discarded. If larger, the extended region is zero-filled.
        [[nodiscard]] virtual auto truncate(uint64_t new_size) -> std::expected<void, Error> = 0;

        /// Acquires a file lock of the given type.
        ///
        /// Lock upgrades must follow the ordering: Shared -> Reserved -> Exclusive.
        /// Attempting to acquire a lock out of order returns an Error.
        [[nodiscard]] virtual auto lock(LockType type) -> std::expected<void, Error> = 0;

        /// Releases any currently held lock, returning the file to the unlocked state.
        [[nodiscard]] virtual auto unlock() -> std::expected<void, Error> = 0;
    };

    /// Lock levels used by the pager to coordinate concurrent database access.
    ///
    /// - Shared:    Allows reading; multiple holders permitted simultaneously.
    /// - Reserved:  Signals intent to write; only one holder at a time.
    /// - Exclusive: Required before writing; no other locks may be held.
    enum class LockType { Shared, Reserved, Exclusive };

    struct OpenOptions {
        bool create    = false;  // create if doesn't exist
        bool read_only = false;  // open in read-only mode
        bool truncate  = false;  // truncate to zero on open
        bool exclusive = false;  // fail if file already exists (O_EXCL)
    };


    [[nodiscard]] auto open(const std::filesystem::path& path, OpenOptions opts)
        -> std::expected<std::unique_ptr<File>, Error>;

} // namespace sqlite::os

#endif //SQLITECPP_FILE_HPP