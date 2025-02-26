cmake_minimum_required(VERSION 3.30)
project(sqlitecpp)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add clang-format target if the program is found
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    # Get all project files
    file(GLOB_RECURSE ALL_SOURCE_FILES
            ${PROJECT_SOURCE_DIR}/src/*.cc
            ${PROJECT_SOURCE_DIR}/include/**/*.h
            ${PROJECT_SOURCE_DIR}/include/*.h
    )

    add_custom_target(
            format
            COMMAND ${CLANG_FORMAT}
            -i
            ${ALL_SOURCE_FILES}
    )
else()
    message(STATUS "clang-format not found, format target not available")
endif()

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

