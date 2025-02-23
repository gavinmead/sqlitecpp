cmake_minimum_required(VERSION 3.30)
project(sqlitecpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Coverage option
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Add coverage flags
    add_compile_options(--coverage -O0)
    add_link_options(--coverage)
    message(STATUS "Code coverage enabled")
endif()

file(GLOB_RECURSE LIBRARY_SOURCES "src/*.cc")

# Create static library
add_library(${PROJECT_NAME} STATIC ${LIBRARY_SOURCES})
target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)


# Testing setup
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
