# OS abstraction layer

add_library(sqlite_os INTERFACE)
add_library(sqlite::os ALIAS sqlite_os)

target_include_directories(sqlite_os INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# When implementation files are added:
# add_library(sqlite_os
#     file.cpp
# )
# target_set_warnings(sqlite_os)
# target_enable_sanitizers(sqlite_os)
