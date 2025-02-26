//
// Created by Gavin Mead on 2/21/25.
//

#include "sqlite.h"
#include <filesystem>
#include <optional>
#include <unistd.h>
#include <fcntl.h>  // for close()
#include <set>
#include <vector>

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
            int db_file_descriptor = ::open(this->db_file.c_str(), O_RDWR | O_CREAT, 0664);
            if (db_file_descriptor == -1) {
                auto result = SQLITE_CANTOPEN;
                this->update_last_message(result, is_error(result), "could not open file");
                return result;
            }
            this->opened = true;

            auto result = this->setup_db_mmap(db_file_descriptor, true);
            if(result != SQLITE_OK) {
                return result;
            }

            result = SQLITE_OK;
            this->update_last_message(result, is_error(result), "new database created successfully");
            return result;

        } else {
            //TODO open the database file

            //TODO: initialize the DB internals

            auto result = SQLITE_OK;
            this->update_last_message(result, is_error(result), "database opened successfully");
            return result;
        }

    }

    SQLITE_RESULT SQLiteConnection::close() {
        //Treat this as an idempotent operation
        //Manually reset the unique pointer to close trigger the unmap
        this->db_file_mmap.reset();
        this->opened = false;
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

    SQLITE_RESULT SQLiteConnection::setup_db_mmap(int db_file_descriptor, bool is_new_db) {
        //TODO: initialize the DB internals


        //If new db truncate based on the provided page size
        off_t size = this->db_header->pageSize();
        if(is_new_db) {
            if (ftruncate(db_file_descriptor, size) == -1) {
                auto result = SQLITE_CANTOPEN;
                this->update_last_message(result, is_error(result), "could not initialize empty db file");
                return result;
            }
        } else {
            //size_t size = lseek(db_file_descriptor, 0, SEEK_END);
            //lseek(db_file_descriptor, 0, SEEK_SET);
        }

        void* mapped = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, db_file_descriptor, 0);
        if(mapped == MAP_FAILED) {
            auto result = SQLITE_CANTOPEN;
            this->update_last_message(result, is_error(result), "could not mmap file: " + std::to_string(errno));
            return result;
        }

        ::close(db_file_descriptor);

        this->db_file_mmap = std::make_unique<std::vector<std::byte>>();
        auto bytes = static_cast<std::byte*>(mapped);
        this->db_file_mmap->get()->assign(bytes, bytes + size);

        return SQLITE_OK;
    }
}

