# SQLiteCpp Makefile
# Simplifies common build operations using CMake presets

.PHONY: all build test clean distclean \
        debug release coverage sanitizer-asan sanitizer-tsan sanitizer-ubsan \
        format lint help

# Default target
all: build

# Build (default: debug)
build:
	cmake --preset dev-debug
	cmake --build --preset dev-debug

# Run tests
test: build
	ctest --preset dev-debug

# Clean build directory
clean:
	rm -rf build cmake-build-*

# Full clean (including generated files)
distclean: clean
	rm -rf CMakeUserPresets.json

#
# Build configurations
#

# Debug build
debug:
	cmake --preset dev-debug
	cmake --build --preset dev-debug

# Release build
release:
	cmake --preset dev-release
	cmake --build --preset dev-release

# Coverage build
coverage:
	cmake --preset coverage
	cmake --build --preset coverage
	ctest --test-dir build/coverage --output-on-failure
	gcovr --root . --exclude tests/ --exclude build/ \
		--html coverage.html --html-details
	@echo "Coverage report: coverage.html"

# Address Sanitizer build
sanitizer-asan:
	cmake --preset sanitizer-asan
	cmake --build --preset sanitizer-asan
	ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build/sanitizer-asan --output-on-failure

# Thread Sanitizer build
sanitizer-tsan:
	cmake --preset sanitizer-tsan
	cmake --build --preset sanitizer-tsan
	ctest --test-dir build/sanitizer-tsan --output-on-failure

# Undefined Behavior Sanitizer build
sanitizer-ubsan:
	cmake --preset sanitizer-ubsan
	cmake --build --preset sanitizer-ubsan
	UBSAN_OPTIONS=print_stacktrace=1 ctest --test-dir build/sanitizer-ubsan --output-on-failure

#
# Code quality
#

# Format code with clang-format
format:
	find src include tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i

# Check formatting (CI)
format-check:
	find src include tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format --dry-run --Werror

# Run clang-tidy (requires compile_commands.json)
lint: build
	find src -name '*.cpp' | xargs clang-tidy -p build/dev-debug

#
# Help
#

help:
	@echo "SQLiteCpp Build System"
	@echo ""
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all              Build the project (default)"
	@echo "  build            Build debug configuration"
	@echo "  test             Build and run tests"
	@echo "  clean            Remove build directories"
	@echo "  distclean        Remove build and generated files"
	@echo ""
	@echo "Build Configurations:"
	@echo "  debug            Debug build"
	@echo "  release          Release build"
	@echo "  coverage         Build with coverage, run tests, generate report"
	@echo "  sanitizer-asan   Build with Address Sanitizer"
	@echo "  sanitizer-tsan   Build with Thread Sanitizer"
	@echo "  sanitizer-ubsan  Build with Undefined Behavior Sanitizer"
	@echo ""
	@echo "Code Quality:"
	@echo "  format           Format code with clang-format"
	@echo "  format-check     Check code formatting"
	@echo "  lint             Run clang-tidy"
