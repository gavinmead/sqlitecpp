# Code coverage support

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(WARNING "Coverage requires GCC or Clang")
    return()
endif()

# Add coverage flags to all targets
add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
add_link_options(--coverage)

# Find gcovr for report generation
find_program(GCOVR gcovr)
if(GCOVR)
    add_custom_target(coverage
        COMMAND ${GCOVR}
            --root ${CMAKE_SOURCE_DIR}
            --exclude ${CMAKE_SOURCE_DIR}/tests
            --exclude ${CMAKE_BINARY_DIR}
            --xml coverage.xml
            --html coverage.html
            --html-details
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating coverage report"
    )
endif()
