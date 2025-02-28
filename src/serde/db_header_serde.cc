
#include "db_header.h"
#include "serde/db_header_serde.h"

namespace sql::serde {
  void DBHeaderSerde::serialize(const std::shared_ptr<stream::ByteOutputStream>& out,
                                const std::shared_ptr<DBHeader>& header) {
    out->writeString(header->name());
    out->write<unsigned short>(header->pageSize(), std::endian::big);
  }

  std::shared_ptr<DBHeader> DBHeaderSerde::deserialize(const std::shared_ptr<stream::ByteInputStream>& in) {
    auto name = in->readString();
    auto page_size = in->read<unsigned short>(std::endian::big);
    return std::make_shared<DefaultDBHeader>(page_size.value(), name);
  }

}