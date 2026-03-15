# SQL parser and tokenizer

add_library(sqlite_sql INTERFACE)
add_library(sqlite::sql ALIAS sqlite_sql)

target_include_directories(sqlite_sql INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

# When implementation files are added:
# add_library(sqlite_sql
#     tokenizer.cpp
#     parser.cpp
#     ast.cpp
# )
# target_set_warnings(sqlite_sql)
# target_enable_sanitizers(sqlite_sql)
