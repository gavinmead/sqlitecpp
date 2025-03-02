//
// Created by Gavin Mead on 3/1/25.
//

#ifndef SQLITECPP_MEMORY_MANAGER_H
#define SQLITECPP_MEMORY_MANAGER_H

#include <list>
#include <memory>
#include <string>
#include "stream/byte_input_stream.h"
#include "stream/byte_output_stream.h"


namespace sql::memory {

class MemoryBlock {
 public:
  MemoryBlock() = default;
  virtual ~MemoryBlock() = default;
//  virtual sql::stream::ByteInputStream getInputStream(
//      unsigned int start_pos,
//      unsigned int end_pos) = 0;
//  virtual sql::stream::ByteOutputStream getOutputStream(
//      unsigned int start_pos,
//      unsigned int end_pos) = 0;

  virtual unsigned int getBlockSize() = 0;
  virtual unsigned int getBlockId() = 0;
  virtual unsigned int getUsedSpace() = 0;
  virtual unsigned int getFreeSpace() = 0;
};

class MemoryPoolManager {
 public:
  virtual ~MemoryPoolManager() = default;
  virtual std::shared_ptr<MemoryBlock> getBlock(unsigned int block_id) = 0;

  virtual unsigned int getBlockSize() = 0;
  virtual unsigned int getBlockCount() = 0;
  virtual unsigned int getInitialBlockCount() = 0;
  virtual void allocateBlocks(unsigned int block_count) = 0;

  // Returns the total bytes in the memory pool
  virtual unsigned int getBlockPoolSize() = 0;

  // Returns the total bytes in the memory pool that are free
  virtual unsigned int getTotalFreeSpace() = 0;

  // Returns the total bytes in the memory pool that are used
  virtual unsigned int getTotalUsedSpace() = 0;

  virtual void initialize() = 0;
};

class MMapMemoryBlock : public MemoryBlock {
 public:
  MMapMemoryBlock(unsigned int block_size, unsigned int block_id);
  ~MMapMemoryBlock() override = default;

//  sql::stream::ByteInputStream getInputStream(
//      unsigned int start_pos,
//      unsigned int end_pos) override;
//
//  sql::stream::ByteOutputStream getOutputStream(
//      unsigned int start_pos,
//      unsigned int end_pos) override;

  unsigned int getBlockSize() override;
  unsigned int getBlockId() override;
  unsigned int getUsedSpace() override;
  unsigned int getFreeSpace() override;

 private:
  unsigned int block_size;
  unsigned int block_id;
  unsigned int used_space;
  //std::shared_ptr<std::vector<std::byte>> buffer;
};

class MMapMemoryPoolManager : public MemoryPoolManager {
 public:
  MMapMemoryPoolManager(
      unsigned int block_size,
      unsigned int initial_block_count,
      std::string filename);

  ~MMapMemoryPoolManager() override;

  std::shared_ptr<MemoryBlock> getBlock(unsigned int block_id) override;
  void allocateBlocks(unsigned int block_count) override;

  void initialize() override;

  unsigned int getBlockSize() override;
  unsigned int getBlockCount() override;
  unsigned int getInitialBlockCount() override;
  unsigned int getBlockPoolSize() override;
  unsigned int getTotalFreeSpace() override;
  unsigned int getTotalUsedSpace() override;

 private:
  unsigned int block_size;
  unsigned int initial_block_count;
  unsigned int block_count;
  std::list<MMapMemoryBlock> memory_blocks;
  std::string filename;
  std::byte* mmap_ptr;
  bool intialized = false;
};

}; // namespace sql::memory

#endif // SQLITECPP_MEMORY_MANAGER_H
