add_sqlite_test(os_error_test
    error_test.cpp
    file_test.cpp
    std_file_test.cpp
)

target_link_libraries(os_error_test PRIVATE
    sqlite::os
    GTest::gmock
)

target_compile_definitions(os_error_test PRIVATE
    TEST_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../../fixtures"
)