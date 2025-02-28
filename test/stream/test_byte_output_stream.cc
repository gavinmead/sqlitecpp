#include <gtest/gtest.h>
#include <ranges>
#include <vector>
#include "stream/byte_output_stream.h"

TEST(OutputByteStream, WriteDefault) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(4));
  out.write<int>(42);
  auto buffer = out.getBytes();
  EXPECT_EQ(buffer->size(), 4); // 4 bytes in an int

  auto bytes = buffer->at(0);
  EXPECT_EQ(to_integer<int>(bytes), 42);
};

TEST(OutputByteStream, WriteWithBigEndian) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(4));
  out.write<int>(42, std::endian::big);
  auto buffer = out.getBytes();
  EXPECT_EQ(buffer->size(), 4); // 4 bytes in an int

  auto bytes = buffer->at(3); // Written to end of the buffer
  EXPECT_EQ(to_integer<int>(bytes), 42);
}

TEST(OutputByteStream, WriteMultiple) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
  out.write<int>(42, std::endian::big);
  out.write<double>(24.24, std::endian::big);
  out.writeString("Hello, World!");
  auto buffer = out.getBytes();
  //Assert big endian by for 42
  EXPECT_EQ((buffer->at(3)), std::byte(42));

};

TEST(OutputByteStream, PositionOutOfBounds) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(0));
  auto result = out.write<int>(42);
  EXPECT_EQ(result, std::unexpected(sql::stream::OutputStreamError::PositionOutOfBounds));
};

TEST(OutputByteStream, InitialState) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
  EXPECT_EQ(out.getCurrentPosition(), 0);
  EXPECT_FALSE(out.isEndOfStream());
};