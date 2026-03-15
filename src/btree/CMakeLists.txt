# B-tree storage engine

add_library(sqlite_btree INTERFACE)
add_library(sqlite::btree ALIAS sqlite_btree)

target_include_directories(sqlite_btree INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(sqlite_btree INTERFACE
    sqlite::pager
)

# When implementation files are added:
# add_library(sqlite_btree
#     btree.cpp
#     cursor.cpp
#     cell.cpp
# )
# target_set_warnings(sqlite_btree)
# target_enable_sanitizers(sqlite_btree)
