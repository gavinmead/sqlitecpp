//
// Created by Gavin Mead on 2/26/25.
//

#ifndef SQLITECPP_BYTE_INPUT_STREAM_H
#define SQLITECPP_BYTE_INPUT_STREAM_H

#include <bit>
#include <cstddef>
#include <memory>
#include <vector>
#include <expected>
#include <algorithm>

namespace sql::stream {

enum class InputStreamStatus {
  Ok,
};

enum class InputStreamError {
  PositionOutOfBounds,
  EndOfStream,
};

class ByteInputStream {
 public:
  explicit ByteInputStream(
      const std::shared_ptr<std::vector<std::byte>>& buffer)
      : buffer(buffer) {};

  template <typename T>
  std::expected<T, InputStreamError> read(std::endian endian = std::endian::native) {
    auto size = sizeof(T);
    //TODO:  Check if the size + buffer_position is greater than the buffer size.  If so we want return an error
    auto bytes =
        reinterpret_cast<std::byte*>(&buffer->at(this->buffer_position));
    T value;

    if (endian == std::endian::big) {
      std::reverse(bytes, bytes + size);
    }

    for (size_t i = 0; i < size; i++) {
      reinterpret_cast<std::byte*>(&value)[i] = bytes[i];
    }
    this->buffer_position += size;
    return value;
  };

  std::string readString() {
    std::string value;
    while (buffer->at(this->buffer_position) != std::byte(0)) {
      value += static_cast<char>(buffer->at(this->buffer_position));
      this->buffer_position++;
    }
    this->buffer_position++;
    return value;
  }

  int getCurrentPosition() {
    return this->buffer_position;
  }

  bool isEndOfStream() {
    return this->buffer_position >= buffer->size();
  }

  void reset() {
    this->buffer_position = 0;
  }

  std::expected<InputStreamStatus, InputStreamError> setCurrentPosition(int position) {
    if (position < 0 || position >= buffer->size()) {
          return std::unexpected(InputStreamError::PositionOutOfBounds);
    }
    this->buffer_position = position;
    return InputStreamStatus::Ok;
  }

 private:
  std::shared_ptr<std::vector<std::byte>> buffer;
  int buffer_position = 0;
};
} // namespace sql::io

#endif // SQLITECPP_BYTE_INPUT_STREAM_H
