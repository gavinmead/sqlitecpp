#ifndef SQLITECPP_MOCK_FILE_HPP
#define SQLITECPP_MOCK_FILE_HPP

#include <gmock/gmock.h>
#include "sqlite/os/file.hpp"

namespace sqlite::os {

    class MockFile : public File {
    public:
        MOCK_METHOD((std::expected<size_t, Error>), read,
                    (std::span<std::byte> buf, uint64_t offset), (override));

        MOCK_METHOD((std::expected<size_t, Error>), write,
                    (std::span<const std::byte> buf, uint64_t offset), (override));

        MOCK_METHOD((std::expected<uint64_t, Error>), size, (), (override));
        MOCK_METHOD((std::expected<void, Error>), sync, (), (override));
        MOCK_METHOD((std::expected<void, Error>), truncate, (uint64_t new_size), (override));
        MOCK_METHOD((std::expected<void, Error>), lock, (LockType type), (override));
        MOCK_METHOD((std::expected<void, Error>), unlock, (), (override));
    };

} // namespace sqlite::os

#endif //SQLITECPP_MOCK_FILE_HPP
