//
// Created by Gavin Mead on 2/25/25.
//

#ifndef SQLITECPP_BYTE_OUTPUT_STREAM_H
#define SQLITECPP_BYTE_OUTPUT_STREAM_H

#include <bit>
#include <cstddef>
#include <memory>
#include <vector>

namespace sql::stream {
class ByteOutputStream {
 public:
  // Switch to a shared_ptr in a future iteration
  explicit ByteOutputStream(const std::vector<std::byte>& buffer) {
    this->buffer = std::make_shared<std::vector<std::byte>>(buffer);
  }

  template <typename T>
  void write(T value, std::endian endian = std::endian::native) {
    auto size = sizeof(T);
    auto bytes = reinterpret_cast<std::byte*>(&value);

    if (endian == std::endian::big) {
      std::reverse(bytes, bytes + size);
    }
    for (size_t i = 0; i < size; i++) {
      buffer->push_back(bytes[i]);
    }
  };

  void writeString(std::string value) {
    for (auto c : value) {
      buffer->push_back(std::byte(c));
    }
    buffer->push_back(std::byte(0));
  }

  std::shared_ptr<std::vector<std::byte>> getBytes() {
    return std::shared_ptr<std::vector<std::byte>>({buffer});
  }

 private:
  std::shared_ptr<std::vector<std::byte>> buffer;
};
}; // namespace sql::stream

#endif // SQLITECPP_BYTE_OUTPUT_STREAM_H
