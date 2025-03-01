#ifndef SQLITECPP_FILE_FIXTURE_H
#define SQLITECPP_FILE_FIXTURE_H

#include <gtest/gtest.h>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <random>
#include <string>

namespace fs = std::filesystem;

class FileSetupFixture : public testing::Test {
 protected:
  void SetUp() override {
    this->dir = fs::temp_directory_path().append(generate_random_string(10));
    fs::create_directory(this->dir);
    this->filename = dir.string() + generate_random_string(10) + ".db3";
  }

  void TearDown() override {
    // Delete the temp directory
    fs::remove_all(this->dir);
  }

  std::string generate_random_string(size_t length) {
    const std::basic_string characters =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string random_string;
    for (size_t i = 0; i < length; ++i) {
      random_string += characters[distribution(generator)];
    }

    return random_string;
  }

  fs::path dir;
  std::string filename;
};

#endif // SQLITECPP_FILE_FIXTURE_H
