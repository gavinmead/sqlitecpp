//
// Created by Gavin Mead on 2/25/25.
//

#ifndef SQLITECPP_BYTE_OUTPUT_STREAM_H
#define SQLITECPP_BYTE_OUTPUT_STREAM_H

#include <bit>
#include <cstddef>
#include <memory>
#include <vector>
#include <expected>

namespace sql::stream {

enum class OutputStreamStatus {
  Ok,
};

enum class OutputStreamError {
  PositionOutOfBounds,

};

class ByteOutputStream {
 public:

  explicit ByteOutputStream(const std::shared_ptr<std::vector<std::byte>>& buffer)
      : buffer(buffer) {}

  template <typename T>
  std::expected<OutputStreamStatus, OutputStreamError> write(T value, std::endian endian = std::endian::native) {
    auto size = sizeof(T);
    auto bytes = reinterpret_cast<std::byte*>(&value);

    if (this->current_position + size > buffer->size()) {
      return std::unexpected(OutputStreamError::PositionOutOfBounds);
    }

    if (endian == std::endian::big) {
      std::reverse(bytes, bytes + size);
    }
    for (size_t i = 0; i < size; i++) {
      buffer->at(this->current_position) = bytes[i];
      this->current_position++;
    }
    return OutputStreamStatus::Ok;
  };

  std::expected<OutputStreamStatus, OutputStreamError> writeString(std::string value) {
    for (auto c : value) {
      buffer->at(this->current_position) = std::byte(c);
      this->current_position++;
    }
    buffer->at(this->current_position) = std::byte(0);
    this->current_position++;
    return OutputStreamStatus::Ok;
  }

  std::shared_ptr<std::vector<std::byte>> getBytes() {
    return std::shared_ptr<std::vector<std::byte>>({buffer});
  }

  int getCurrentPosition() {
    return this->current_position;
  }

  bool isEndOfStream() {
    return this->current_position >= buffer->size();
  }

 private:
  std::shared_ptr<std::vector<std::byte>> buffer;
  int current_position = 0;
};
}; // namespace sql::stream

#endif // SQLITECPP_BYTE_OUTPUT_STREAM_H
