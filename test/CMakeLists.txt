# Use GoogleTest for testing
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(unit_tests test_example.cc)
target_link_libraries(unit_tests PRIVATE
        ${PROJECT_NAME}  # Link to our static library
        gtest_main
)

# Register tests
include(GoogleTest)
gtest_discover_tests(unit_tests)