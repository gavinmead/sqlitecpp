#include <gtest/gtest.h>
#include <ranges>
#include <vector>
#include "stream/byte_output_stream.h"

TEST(OutputByteStream, WriteDefault) {
  auto out = sql::stream::ByteOutputStream(std::vector<std::byte>());
  out.write<int>(42);
  auto buffer = out.getBytes();
  EXPECT_EQ(buffer->size(), 4); // 4 bytes in an int

  auto bytes = buffer->at(0);
  EXPECT_EQ(to_integer<int>(bytes), 42);
};

TEST(OutputByteStream, WriteWithBigEndian) {
  auto out = sql::stream::ByteOutputStream(std::vector<std::byte>());
  out.write<int>(42, std::endian::big);
  auto buffer = out.getBytes();
  EXPECT_EQ(buffer->size(), 4); // 4 bytes in an int

  auto bytes = buffer->at(3); // Written to end of the buffer
  EXPECT_EQ(to_integer<int>(bytes), 42);
}

TEST(OutputByteStream, WriteMultiple) {
  auto out = sql::stream::ByteOutputStream(std::vector<std::byte>());
  out.write<int>(42, std::endian::big);
  out.write<double>(24.24, std::endian::big);
  out.writeString("Hello, World!");
  auto buffer = out.getBytes();
  EXPECT_EQ(buffer->size(), 4 + 8 + 14); // 4 bytes in an int, 8 bytes in a double, 13 bytes in the string
}