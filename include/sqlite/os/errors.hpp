#ifndef SQLITECPP_ERRORS_HPP
#define SQLITECPP_ERRORS_HPP

/// @file errors.hpp
/// @brief OS-layer error types for SQLiteCpp.
///
/// Defines the error codes and `Error` struct used throughout the OS abstraction
/// layer. All fallible OS operations return `std::expected<T, Error>` rather than
/// throwing exceptions, keeping error propagation explicit and zero-cost on the
/// happy path.
///
/// ### Usage
/// ```cpp
/// auto result = pager.read_page(id);
/// if (!result) {
///     auto& err = result.error();
///     std::println("read failed: {} at {}:{}", err.message,
///                  err.location.file_name(), err.location.line());
/// }
/// ```
///
/// ### Adding new error codes
/// Add the enumerator to `ErrorCode` and update any switch statements that
/// handle the full set of codes.

#include <source_location>
#include <string>

namespace sqlite::os {

    /// @brief Categorises the kind of OS-level failure that occurred.
    ///
    /// Callers use this to branch on recoverable vs. fatal errors without
    /// string-matching the human-readable message.
    enum class ErrorCode {
        IoError,          ///< Generic I/O failure (read/write syscall returned an error).
        FileNotFound,     ///< The requested file or path does not exist.
        PermissionDenied, ///< The process lacks the required filesystem permissions.
        AlreadyExists,    ///< A create operation targeted a path that already exists.
        OutOfMemory,      ///< Memory allocation failed.
        InvalidArgument,  ///< A caller-supplied argument was out of range or malformed.
        NotSupported,     ///< The operation is not supported on this platform or configuration.
    };

    /// @brief Carries a categorised error together with a human-readable message
    ///        and the call-site location where the error was created.
    ///
    /// Prefer constructing values of this type via `make_error()` so that
    /// `std::source_location::current()` is captured at the correct call site.
    struct Error {
        ErrorCode code;               ///< Machine-readable error category.
        std::string message;          ///< Human-readable description of the failure.
        std::source_location location; ///< Source location where the error was created.
    };

    /// @brief Constructs an `Error` capturing the call-site source location.
    ///
    /// The default argument for `loc` is evaluated at the call site (not inside
    /// this function), so the captured file name, function name, and line number
    /// reflect where `make_error` was called — not where it is defined.
    ///
    /// @param code     The error category.
    /// @param message  A human-readable description of the failure.
    /// @param loc      Automatically captured call-site location; do not pass explicitly.
    /// @return         A fully populated `Error`.
    [[nodiscard]] Error make_error(ErrorCode code, std::string_view message,
                                   std::source_location loc = std::source_location::current());

} // namespace sqlite::os

#endif //SQLITECPP_ERRORS_HPP