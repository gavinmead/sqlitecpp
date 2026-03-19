# OS abstraction layer

add_library(sqlite_os
      errors.cpp
      std_file.cpp
)
add_library(sqlite::os ALIAS sqlite_os)

target_include_directories(sqlite_os PUBLIC
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_set_warnings(sqlite_os)

if(SQLITECPP_ENABLE_SANITIZERS)
  target_enable_sanitizers(sqlite_os)
endif()
