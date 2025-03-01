#include "memorymgr/memory_manager.h"

#include <utility>
#include <filesystem>
#include <fcntl.h> // for close()
#include <unistd.h>
#include <sys/mman.h>
#include <exception>

namespace fs = std::filesystem;

namespace sql::memory {

MMapMemoryPoolManager::MMapMemoryPoolManager(
    unsigned int block_size,
    unsigned int initial_block_count,
    std::string filename) {
  this->block_size = block_size;
  this->initial_block_count = initial_block_count;
  this->block_count = initial_block_count;
  this->filename = std::move(filename);
  this->intialized = false;
  this->memory_blocks = std::list<MMapMemoryBlock>{};
};

void MMapMemoryPoolManager::initialize() {
  // Open up the file and create the initial blocks as defined by
  // initial_block_count * block_size
  // Truncate the file if it does not exist
  // Create the mmap and store the mmap pointer
  auto file_size = this->block_size * this->initial_block_count;
  if (!fs::exists(this->filename)) {
    int fd = ::open(this->filename.c_str(), O_RDWR | O_CREAT, 0664);
    if (fd == -1) {
      throw fs::filesystem_error(
          "could not open file", std::make_error_code(std::errc::io_error));
    }

    if (ftruncate(fd, file_size) == -1) {
      throw fs::filesystem_error(
          "could not initialize backing file", std::make_error_code(std::errc::io_error));
    }

    //Create the mmap file
    this->mmap_ptr = static_cast<std::byte*>(
        mmap(nullptr, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

    if (this->mmap_ptr == MAP_FAILED) {
      throw std::runtime_error("could not create mmap for file " + this->filename);
    }

    //TODO: Setup the memory blocks based on the mmap

    this->intialized = true;
  }
}
std::shared_ptr<MemoryBlock> MMapMemoryPoolManager::getBlock(
    unsigned int block_id) {
  return std::shared_ptr<MemoryBlock>();
}
void MMapMemoryPoolManager::allocateBlocks(unsigned int num_blocks) {}

unsigned int MMapMemoryPoolManager::getBlockSize() {
  return 0;
}
unsigned int MMapMemoryPoolManager::getBlockCount() {
  return 0;
}
unsigned int MMapMemoryPoolManager::getInitialBlockCount() {
  return 0;
}
unsigned int MMapMemoryPoolManager::getBlockPoolSize() {
  return 0;
}
unsigned int MMapMemoryPoolManager::getTotalFreeSpace() {
  return 0;
}
unsigned int MMapMemoryPoolManager::getTotalUsedSpace() {
  return 0;
}

unsigned int MMapMemoryBlock::getBlockSize() {
  return 0;
}
unsigned int MMapMemoryBlock::getBlockId() {
  return 0;
}
unsigned int MMapMemoryBlock::getUsedSpace() {
  return 0;
}
unsigned int MMapMemoryBlock::getFreeSpace() {
  return 0;
}

MMapMemoryBlock::MMapMemoryBlock(
    unsigned int block_size,
    unsigned int block_id) {}

}; // namespace sql::memory
