//
// Created by Gavin Mead on 2/27/25.
//

#ifndef SQLITECPP_DB_HEADER_SERDE_H
#define SQLITECPP_DB_HEADER_SERDE_H

#include "db_header.h"
#include "stream/byte_input_stream.h"
#include "stream/byte_output_stream.h"
#include <memory>


namespace sql::serde {
class DBHeaderSerde {
 public:
  DBHeaderSerde() = default;
  ~DBHeaderSerde() = default;

  void serialize(const std::shared_ptr<stream::ByteOutputStream>& out,
                 const std::shared_ptr<DBHeader>& header);
  std::shared_ptr<DBHeader> deserialize(const std::shared_ptr<stream::ByteInputStream>& in);

};
};


#endif // SQLITECPP_DB_HEADER_SERDE_H
