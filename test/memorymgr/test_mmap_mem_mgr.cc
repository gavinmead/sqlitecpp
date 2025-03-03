#include <gtest/gtest.h>
#include "memorymgr/memory_manager.h"
#include "../file_fixture.h"

namespace smem = sql::memory;

TEST_F(FileSetupFixture, MemoryManagerInitializeOk) {
  {
    auto memory_manager= std::make_unique<smem::MMapMemoryPoolManager>(512, 2, filename);
    ASSERT_NO_THROW({
      memory_manager->initialize();
    });
    //Assert the file size is 1024
    auto file_size = fs::file_size(filename);
    EXPECT_EQ(file_size, 1024); //512 * 2 = 1024
  }
};


