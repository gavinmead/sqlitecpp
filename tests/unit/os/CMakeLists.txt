add_sqlite_test(os_error_test
    error_test.cpp
    file_test.cpp
)

target_link_libraries(os_error_test PRIVATE
    sqlite::os
    GTest::gmock
)