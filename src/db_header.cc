//
// Created by Gavin Mead on 2/22/25.
//

#include "db_header.h"

namespace sql {

std::string DefaultDBHeader::name() {
  return this->header_name;
}

int DefaultDBHeader::pageSize() {
  if (this->page_size ==
      1) { // See https://www.sqlite.org/fileformat2.html#page_size
    return 65536;
  } else {
    return this->page_size;
  }
}

} // namespace sql
