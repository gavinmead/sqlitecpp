//
// The main interfaces for sqlite++
//

#ifndef SQLITECPP_SQLITE_H
#define SQLITECPP_SQLITE_H

#pragma once

#include <string>
#include <utility>
#include <optional>
#include <fstream>
#include <memory>
#include "db_header.h"
#include <sys/mman.h>
#include <utility>
#include <vector>

namespace sql {

    using SQLITE_RESULT = int;

//Response codes
//The SQLITE_OK result code means that the operation was successful and that there were no errors. Most other result codes indicate an error.
    constexpr SQLITE_RESULT SQLITE_OK = 0;

//The SQLITE_CANTOPEN result code indicates that SQLite was unable to open a file. The file in question might be a primary database file or one of several temporary disk files.
    constexpr SQLITE_RESULT SQLITE_CANTOPEN = 14;

    //end SQLITE_RESULT

    //Page size for the SQLite database
    using SQLITE_PAGE_SIZE = unsigned short;
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_512 = 512;
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_1024 = 1024;
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_2048 = 2048;
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_4096 = 4096;
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_8192 = 8192;
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_16384 = 16384;
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_32768 = 32768;
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_65536 = 1; //See https://www.sqlite.org/fileformat2.html#page_size
    constexpr SQLITE_PAGE_SIZE SQLITE_PAGE_SIZE_DEFAULT = SQLITE_PAGE_SIZE_4096;
    //End SQLITE_PAGE_SIZE

    struct SQLiteMessage {
        SQLITE_RESULT result{};
        bool isError{};
        std::string message;
    };

    class SQLiteConnection {
    public:
        explicit SQLiteConnection(std::string filename, SQLITE_PAGE_SIZE pageSize = SQLITE_PAGE_SIZE_DEFAULT)
                : db_file(std::move(filename)) {
            last_message = std::make_unique<SQLiteMessage>();
            last_message_available = false;
            db_header = std::make_shared<DefaultDBHeader>(pageSize);

        }

        SQLITE_RESULT open();

        SQLITE_RESULT close();

        std::optional<std::shared_ptr<DBHeader>> header() {
            if(opened) {
                return db_header;
            } else {
                return std::nullopt;
            }
        }

        std::optional<SQLiteMessage> lastMessage();

    private:

        SQLITE_RESULT setup_db_mmap(int db_file_descriptor, bool is_new_db);

        void update_last_message(SQLITE_RESULT result, bool isError, std::string message) {
            this->last_message_available = true;
            this->last_message->result = result;
            this->last_message->isError = isError;
            this->last_message->message = std::move(message);
        }

        //Filename used to either create or open an existing SQLite database
        std::string db_file;
        bool opened{false};
        std::optional<std::unique_ptr<std::vector<std::byte>>> db_file_mmap; //We make optional to delay initialization until open
        bool last_message_available;
        std::unique_ptr<SQLiteMessage> last_message;
        std::shared_ptr<DBHeader> db_header;
    };
};



#endif //SQLITECPP_SQLITE_H
