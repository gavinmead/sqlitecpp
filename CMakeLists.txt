cmake_minimum_required(VERSION 3.21)

project(sqlitecpp
    VERSION 0.1.0
    DESCRIPTION "Educational SQLite implementation in modern C++"
    LANGUAGES CXX
)

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(SQLITECPP_BUILD_TESTS "Build tests" ON)
option(SQLITECPP_BUILD_TOOLS "Build CLI tools" ON)
option(SQLITECPP_ENABLE_COVERAGE "Enable code coverage" OFF)
option(SQLITECPP_ENABLE_SANITIZERS "Enable sanitizers" OFF)
set(SQLITECPP_SANITIZER "address" CACHE STRING "Which sanitizer to use (address, thread, undefined)")

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Compiler warnings
include(CompilerWarnings)

# Sanitizers (if enabled)
if(SQLITECPP_ENABLE_SANITIZERS)
    include(sanitizers)
endif()

# Coverage (if enabled)
if(SQLITECPP_ENABLE_COVERAGE)
    include(coverage)
endif()

# Main library components
add_subdirectory(src/os)
add_subdirectory(src/pager)
add_subdirectory(src/btree)
add_subdirectory(src/sql)
add_subdirectory(src/vm)
add_subdirectory(src/sqlite)

# Tests
if(SQLITECPP_BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_subdirectory(tests)
endif()

# Tools
if(SQLITECPP_BUILD_TOOLS)
    add_subdirectory(tools)
endif()
