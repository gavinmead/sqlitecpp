//
// Created by Gavin Mead on 2/21/25.
//

#include "sqlite.h"
#include <filesystem>
#include <optional>
#include <fstream>
#include <set>

namespace fs = std::filesystem;

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
        this->db_file_stream = std::ofstream(this->db_file, std::ios::binary);

        //TODO: initialize the DB internals

        auto result = SQLITE_OK;
        this->update_last_message(result, is_error(result), "new database created successfully");
        return result;

    } else {
        //Open the database file
        this->db_file_stream = std::ofstream(this->db_file, std::ios::binary);

        //TODO: initialize the DB internals
        auto result = SQLITE_OK;
        this->update_last_message(result, is_error(result), "database opened successfully");
        return result;
    }

}

SQLITE_RESULT SQLiteConnection::close() {
    //Treat this as an idempotent operation
    if(this->db_file_stream) {
        if(this->db_file_stream.is_open()) {
            this->db_file_stream.close();
        }
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