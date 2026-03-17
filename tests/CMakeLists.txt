# Test suite

include(GoogleTest)

# Create a test executable helper function
function(add_sqlite_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE
        sqlite::core
        GTest::gtest
        GTest::gtest_main
    )
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_FUNCTION_LIST_DIR})
    target_set_warnings(${name})
    if(SQLITECPP_ENABLE_SANITIZERS)
        target_enable_sanitizers(${name})
    endif()
    gtest_discover_tests(${name})
endfunction()

# Unit tests
add_subdirectory(unit)

# Integration tests
# add_subdirectory(integration)

# Placeholder test to verify the build works
#add_executable(placeholder_test
#    placeholder_test.cpp
#)
#target_link_libraries(placeholder_test PRIVATE
#    GTest::gtest
#    GTest::gtest_main
#)
#gtest_discover_tests(placeholder_test)
