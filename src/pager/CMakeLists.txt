# Pager component - page cache and I/O

add_library(sqlite_pager INTERFACE)
add_library(sqlite::pager ALIAS sqlite_pager)

target_include_directories(sqlite_pager INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(sqlite_pager INTERFACE
    sqlite::os
)

# When implementation files are added:
# add_library(sqlite_pager
#     pager.cpp
#     page.cpp
#     page_cache.cpp
# )
# target_set_warnings(sqlite_pager)
# target_enable_sanitizers(sqlite_pager)
