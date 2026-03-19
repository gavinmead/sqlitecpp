#ifndef SQLITECPP_STD_FILE_HPP
#define SQLITECPP_STD_FILE_HPP

#include <utility>

#include "sqlite/os/errors.hpp"
#include "sqlite/os/file.hpp"

namespace sqlite::os {
    class StdFile : public File {
    public:

        ~StdFile() override;
        StdFile(const StdFile&) = delete;
        StdFile& operator=(const StdFile&) = delete;
        StdFile(StdFile&&) = delete;
        StdFile& operator=(StdFile&&) = delete;


        [[nodiscard]] auto read(std::span<std::byte> buf, uint64_t offset)
            -> std::expected<size_t, Error> override;

        [[nodiscard]] auto write(std::span<const std::byte> buf, uint64_t offset)
            -> std::expected<size_t, Error> override;

        [[nodiscard]] auto size() -> std::expected<uint64_t, Error> override;

        [[nodiscard]] auto sync() -> std::expected<void, Error> override;

        [[nodiscard]] auto truncate(uint64_t new_size) -> std::expected<void, Error> override;

        [[nodiscard]] auto lock(LockType type) -> std::expected<void, Error> override;

        [[nodiscard]] auto unlock() -> std::expected<void, Error> override;

        friend auto open(const std::filesystem::path& path, OpenOptions opts)
            -> std::expected<std::unique_ptr<File>, Error>;

    private:
        explicit StdFile(int fd, std::filesystem::path path, bool is_read_only) {
            fd_ = fd;
            path_ = std::move(path);
            is_read_only_ = is_read_only;
        };


        int fd_;    // The file descriptor for the file
        std::filesystem::path path_;    // path of the file.  useful for error messages.
        bool is_read_only_;

    };

}

#endif //SQLITECPP_STD_FILE_HPP