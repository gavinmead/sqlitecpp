#include <gtest/gtest.h>
#include "stream/byte_output_stream.h"
#include <vector>
#include <ranges>

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

    auto bytes = buffer->at(3); //Written to end of the buffer
    EXPECT_EQ(to_integer<int>(bytes), 42);
}