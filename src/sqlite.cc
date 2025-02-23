//
// Created by Gavin Mead on 2/21/25.
//

#include "sqlite.h"
#include <filesystem>
#include <optional>
#include <unistd.h>
#include <fcntl.h>  // for close()
#include <set>

namespace fs = std::filesystem;

namespace sql {
    std::set<SQLITE_RESULT> errors = {
            SQLITE_CANTOPEN,
    };

    bool is_error(SQLITE_RESULT result) {
        return errors.contains(result);
    }

    SQLITE_RESULT SQLiteConnection::open() {

        //see if the file exists. If so, try to open it, else create a new file
        if(!fs::exists(this->db_file)) {
            //TODO: Make sure it is a regular file
            this->db_file_descriptor = ::open(this->db_file.c_str(), O_RDWR | O_CREAT, 0664);
            if (this->db_file_descriptor == -1) {
                auto result = SQLITE_CANTOPEN;
                this->update_last_message(result, is_error(result), "could not open file");
                return result;
            }
            this->opened = true;
            //TODO: initialize the DB internals

            auto result = SQLITE_OK;
            this->update_last_message(result, is_error(result), "new database created successfully");
            return result;

        } else {
            //open the database file

            //TODO: initialize the DB internals
            auto result = SQLITE_OK;
            this->update_last_message(result, is_error(result), "database opened successfully");
            return result;
        }

    }

    SQLITE_RESULT SQLiteConnection::close() {
        //Treat this as an idempotent operation
        //Close the file descriptor and the mmap
        if (this->db_file_descriptor != -1) {
            ::close(this->db_file_descriptor);
            this->db_file_descriptor = -1;
            this->opened = false;
        }
        auto result = SQLITE_OK;
        this->update_last_message(result, is_error(result), "database closed");
        return SQLITE_OK;
    }

    std::optional<SQLiteMessage> SQLiteConnection::lastMessage() {
        if (this->last_message_available) {
            return SQLiteMessage{
                    this->last_message->result,
                    is_error(this->last_message->result),
                    this->last_message->message
            };
        } else {
            return std::nullopt;
        }
    }
}

