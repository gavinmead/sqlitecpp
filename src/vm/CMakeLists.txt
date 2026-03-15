# Virtual machine (VDBE)

add_library(sqlite_vm INTERFACE)
add_library(sqlite::vm ALIAS sqlite_vm)

target_include_directories(sqlite_vm INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(sqlite_vm INTERFACE
    sqlite::btree
    sqlite::sql
)

# When implementation files are added:
# add_library(sqlite_vm
#     vm.cpp
#     codegen.cpp
# )
# target_set_warnings(sqlite_vm)
# target_enable_sanitizers(sqlite_vm)
