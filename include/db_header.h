#ifndef SQLITECPP_DB_HEADER_H
#define SQLITECPP_DB_HEADER_H

#include <string>
#include <utility>

namespace sql {
class DBHeader {
 public:
  virtual std::string name() = 0;
  virtual int pageSize() = 0;
  virtual ~DBHeader() = default;
};

class DefaultDBHeader : public DBHeader {
 public:
  explicit DefaultDBHeader(unsigned short page_size,
                           std::string magic_string="SQLite format 3\0"):
        page_size(page_size), header_name(std::move(magic_string)) {}
  std::string name() override;
  int pageSize() override;

 private:
  std::string header_name;
  unsigned short page_size = {};
};
} // namespace sql

#endif // SQLITECPP_DB_HEADER_H
