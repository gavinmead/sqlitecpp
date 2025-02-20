#!/bin/bash
set -e

# Create and enter build directory
mkdir -p build_coverage
cd build_coverage

# Configure CMake with coverage enabled
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON

# Build and run tests
cmake --build .
ctest --output-on-failure

# Generate coverage report
lcov --capture --directory .. --output-file coverage.info
lcov --remove coverage.info '/usr/*' '*/test/*' '*/build*/*' '*/build_coverage/_deps/*' --output-file coverage.info

# Generate HTML report
genhtml coverage.info --output-directory coverage_report

echo "Coverage report generated at build_coverage/coverage_report/index.html"
