//
// The main interfaces for sqlite++
//

#ifndef SQLITECPP_SQLITE_H
#define SQLITECPP_SQLITE_H

#pragma once

#include <string>
#include <utility>


class SQLiteConnection {
public:
    SQLiteConnection(std::string filename)
        : db_file(std::move(filename)) {}

private:
    //Filename used to either create or open an existing SQLite database
    std::string db_file;

};


#endif //SQLITECPP_SQLITE_H
