#include <gtest/gtest.h>
#include <ranges>
#include <vector>
#include "stream/byte_output_stream.h"
#include "stream/byte_input_stream.h"

TEST(ByteInputStream, ReadInt) {
      auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
      out.write<int>(42);
      auto buffer = out.getBytes();
      auto in = sql::stream::ByteInputStream(buffer);
      auto value = in.read<int>();
      EXPECT_EQ(value, 42);
}

TEST(ByteInputStream, ReadIntWithBigEndian) {
      auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
      out.write<int>(42, std::endian::big);
      auto buffer = out.getBytes();
      auto in = sql::stream::ByteInputStream(buffer);
      auto value = in.read<int>(std::endian::big);
      EXPECT_EQ(value, 42);
}

TEST(ByteInputStream, ReadNumbers) {
      auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
      out.write<int>(42, std::endian::big);
      out.write<double>(24.24, std::endian::big);
      auto buffer = out.getBytes();
      auto in = sql::stream::ByteInputStream(buffer);
      EXPECT_EQ(in.read<int>(std::endian::big), 42);
      EXPECT_EQ(in.read<double>(std::endian::big), 24.24);
}

TEST(ByteInputStream, ReadString) {
      auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
      out.writeString("Hello, World!");
      auto buffer = out.getBytes();
      auto in = sql::stream::ByteInputStream(buffer);
      EXPECT_EQ(in.readString(), "Hello, World!");
}

TEST(ByteInputStream, ReadStringWithNumbers) {
      auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
      out.writeString("Hello, World!");
      out.write<int>(42);
      auto buffer = out.getBytes();
      auto in = sql::stream::ByteInputStream(buffer);
      EXPECT_EQ(in.readString(), "Hello, World!");
      EXPECT_EQ(in.read<int>(), 42);
}

TEST(ByteInputStream, ChangePosition) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
  out.write<int>(42);
  out.writeString("Hello, World!");
  auto buffer = out.getBytes();
  auto in = sql::stream::ByteInputStream(buffer);

  auto result = in.setCurrentPosition(11);
  EXPECT_EQ(result, sql::stream::InputStreamStatus::Ok);
  EXPECT_EQ(in.readString(), "World!");

}

TEST(ByteInputStream, ChangePositionOutOfBounds) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
  out.write<int>(42);
  out.writeString("Hello, World!");
  auto buffer = out.getBytes();
  auto in = sql::stream::ByteInputStream(buffer);

  auto result = in.setCurrentPosition(513);
  EXPECT_EQ(result, std::unexpected(sql::stream::InputStreamError::PositionOutOfBounds));
  EXPECT_EQ(in.getCurrentPosition(), 0);
}

TEST(ByteInputStream, EndOfStream) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(18));
  out.write<int>(42);
  out.writeString("Hello, World!");
  auto buffer = out.getBytes();
  auto in = sql::stream::ByteInputStream(buffer);

  EXPECT_EQ(in.isEndOfStream(), false);
  EXPECT_EQ(in.read<int>(), 42);
  EXPECT_EQ(in.readString(), "Hello, World!");
  EXPECT_EQ(in.isEndOfStream(), true);
}

TEST(ByteInputStream, Reset) {
  auto out = sql::stream::ByteOutputStream(std::make_shared<std::vector<std::byte>>(512));
  out.write<int>(42);
  auto buffer = out.getBytes();
  auto in = sql::stream::ByteInputStream(buffer);
  EXPECT_EQ(in.read<int>(), 42);

}