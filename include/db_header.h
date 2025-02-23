#ifndef SQLITECPP_DB_HEADER_H
#define SQLITECPP_DB_HEADER_H

#include <string>

namespace sql {
    class DBHeader {
    public:
        virtual std::string name() = 0;
        virtual int pageSize() = 0;
        virtual ~DBHeader() = default;
    };

    class DefaultDBHeader : public DBHeader {
    public:
        explicit DefaultDBHeader(unsigned short page_size) : page_size(page_size) {};
        std::string name() override;
        int pageSize() override;
    private:
        unsigned short page_size={};
    };
}

#endif //SQLITECPP_DB_HEADER_H
