# Top-level SQLite API

add_library(sqlite_core INTERFACE)
add_library(sqlite::core ALIAS sqlite_core)

target_include_directories(sqlite_core INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(sqlite_core INTERFACE
    sqlite::vm
)

# When implementation files are added:
# add_library(sqlite_core
#     database.cpp
#     statement.cpp
# )
# target_set_warnings(sqlite_core)
# target_enable_sanitizers(sqlite_core)
