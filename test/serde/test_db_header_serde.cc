#include <gtest/gtest.h>
#include "serde/db_header_serde.h"
#include "stream/byte_output_stream.h"
#include "db_header.h"

TEST(DBHeaderSerde, SerdeOk) {
  auto db_header = std::make_shared<sql::DefaultDBHeader>(512);
  auto out = std::make_shared<sql::stream::ByteOutputStream>(std::make_shared<std::vector<std::byte>>(512));

  auto db_header_serde = sql::serde::DBHeaderSerde();
  db_header_serde.serialize(out, db_header);

  auto in = std::make_shared<sql::stream::ByteInputStream>(out->getBytes());
  auto deserialized = db_header_serde.deserialize(in);
  ASSERT_EQ(deserialized->pageSize(), 512);
  ASSERT_EQ(deserialized->name(), "SQLite format 3\0");
};