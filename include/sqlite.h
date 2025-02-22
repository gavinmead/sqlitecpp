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

namespace sql {
    using SQLITE_RESULT = int;

//Response codes
//The SQLITE_OK result code means that the operation was successful and that there were no errors. Most other result codes indicate an error.
    constexpr SQLITE_RESULT SQLITE_OK = 0;

//The SQLITE_CANTOPEN result code indicates that SQLite was unable to open a file. The file in question might be a primary database file or one of several temporary disk files.
    constexpr SQLITE_RESULT SQLITE_CANTOPEN = 14;

    struct SQLiteMessage {
        SQLITE_RESULT result;
        bool isError;
        std::string message;
    };

    class SQLiteConnection {
    public:
        explicit SQLiteConnection(std::string filename)
                : db_file(std::move(filename)) {
            last_message = std::make_unique<SQLiteMessage>();
            last_message_available = false;
        }

        SQLITE_RESULT open();

        SQLITE_RESULT close();

        std::optional<SQLiteMessage> lastMessage();

    private:

        void update_last_message(SQLITE_RESULT result, bool isError, std::string message) {
            this->last_message_available = true;
            this->last_message->result = result;
            this->last_message->isError = isError;
            this->last_message->message = std::move(message);
        }

        //Filename used to either create or open an existing SQLite database
        std::string db_file;
        int db_file_descriptor;
        bool last_message_available;
        std::unique_ptr<SQLiteMessage> last_message;
    };
};



#endif //SQLITECPP_SQLITE_H
