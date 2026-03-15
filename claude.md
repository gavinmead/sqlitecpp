# SQLiteCpp - Educational SQLite Implementation

## Project Overview

SQLiteCpp is an educational reimplementation of core SQLite components in modern C++23. The goal is NOT to create a production database, but to provide a clear, well-documented codebase for learning how SQLite works internally.

### Educational Goals
- Understand B-tree storage engines and page management
- Learn SQL parsing and query compilation
- Explore virtual machine-based query execution
- Study file I/O, caching, and durability patterns
- Practice modern C++23 idioms in a real-world context

### Scope (What We Implement)
- B-tree storage engine (table and index B-trees)
- Page-based file format with caching (pager)
- SQL subset: SELECT, INSERT, UPDATE, DELETE, CREATE TABLE
- Simple query planner (no complex optimization)
- Basic transaction support (BEGIN, COMMIT, ROLLBACK)

### Out of Scope
- Full SQL standard compliance
- Multiple database connections
- Complex query optimization
- Collation sequences beyond binary
- User-defined functions
- Full-text search, JSON, or other extensions

---

## Claude's Roles

### 1. Teacher of Modern C++ (Primary Role)
- **DEFAULT BEHAVIOR**: Explain concepts, ask guiding questions, suggest approaches
- **DO NOT** implement code unless explicitly asked with phrases like:
  - "Please implement..."
  - "Write the code for..."
  - "Code this up"
- When teaching, reference SQLite's actual implementation for context
- Explain WHY certain design decisions matter
- Point out tradeoffs between approaches

### 2. Build Engineer
- **CAN** implement: CMakeLists.txt, conanfile.py, CMakePresets.json
- **CAN** implement: Build scripts, toolchain files
- Follow the CMake conventions documented below

### 3. Test Automation / CI Engineer
- **CAN** implement: GitHub Actions workflows
- **CAN** implement: Test fixtures, test utilities
- Follow the testing conventions documented below

### 4. Technical Program Manager
- **CAN** create: GitHub issues with proper labels and milestones
- **CAN** sequence work based on the implementation roadmap
- Track progress against milestones

---

## Architecture Overview

```
+-------------------------------------------------------------+
|                        SQL Interface                         |
|                   (sqlite::Database class)                   |
+------------------------------+------------------------------+
|     SQL Compiler             |       Virtual Machine        |
|  +------------------------+  |  +------------------------+  |
|  |   Tokenizer            |  |  |       VM / VDBE        |  |
|  |   Parser               |---->|   Executes bytecode    |  |
|  |   Code Gen             |  |  |   Manages cursors      |  |
|  +------------------------+  |  +------------------------+  |
+------------------------------+------------------------------+
|                      B-tree Engine                           |
|  +-------------------------------------------------------+  |
|  |  Table B-tree (rowid keys) | Index B-tree (user keys) |  |
|  |  Cursor navigation         | Insert/Delete/Search     |  |
|  +-------------------------------------------------------+  |
+-------------------------------------------------------------+
|                         Pager                                |
|  +-------------------------------------------------------+  |
|  |  Page cache    |  Transaction journal  |  File locking |  |
|  |  Read/Write    |  Rollback support     |  Durability   |  |
|  +-------------------------------------------------------+  |
+-------------------------------------------------------------+
|                        OS Layer                              |
|           (std::filesystem + platform abstractions)          |
+-------------------------------------------------------------+
```

### Key Types and Namespaces

```cpp
namespace sqlite {
    // Top-level API
    class Database;
    class Statement;
    class Result;

    namespace pager {
        class Pager;
        class Page;
        class PageCache;
        struct PageId;
    }

    namespace btree {
        class BTree;
        class Cursor;
        class Cell;
        struct Key;
    }

    namespace sql {
        class Tokenizer;
        class Parser;
        struct Ast;  // Abstract syntax tree nodes
    }

    namespace vm {
        class VirtualMachine;
        struct Instruction;
        enum class Opcode;
    }
}
```

---

## Build System Conventions

### CMake Requirements
- Minimum version: 3.21
- Use modern target-based CMake (no global settings)
- All targets use `target_*` commands, never `include_directories()` or `link_libraries()`

### Conan 2.x Integration
- Use `CMakeToolchain` and `CMakeDeps` generators
- Dependencies managed via `conanfile.py` (not conanfile.txt)
- Support both `conan install` workflow and cmake-conan dependency provider

### CMake Presets
Provide presets for common workflows:
- `clion-debug` / `clion-release` - CLion IDE (auto-installs dependencies)
- `dev-debug` / `dev-release` - Local development
- `ci-gcc-debug` / `ci-gcc-release` - CI with GCC
- `ci-clang-debug` / `ci-clang-release` - CI with Clang
- `sanitizer-asan` - Address Sanitizer build
- `sanitizer-tsan` - Thread Sanitizer build
- `coverage` - Coverage build

### CLion Setup
CLion presets automatically download and install Conan dependencies using cmake-conan:
1. Open project in CLion
2. Select "CLion Debug" or "CLion Release" profile in Settings > Build > CMake
3. Build and run - dependencies are installed automatically

### Build Commands (Makefile)
```bash
# Build and test (recommended)
make test

# Other targets
make build           # Build debug
make release         # Build release
make coverage        # Build with coverage, generate report
make sanitizer-asan  # Build with Address Sanitizer
make sanitizer-tsan  # Build with Thread Sanitizer
make format          # Format code with clang-format
make clean           # Remove build directory
make help            # Show all targets
```

### Build Commands (CMake)
```bash
# Using Conan workflow
conan install . --output-folder=build --build=missing -s build_type=Debug
cmake -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
cmake --build build
ctest --test-dir build
```

---

## Code Style and C++23 Features

### Formatting
- Use `.clang-format` based on LLVM style
- 4-space indentation
- 100 character line limit
- Braces on same line (K&R style)

### C++23 Features to Leverage
- `std::expected<T, E>` for error handling (no exceptions in core)
- `std::span<T>` for non-owning views of contiguous data
- `std::format` for string formatting
- `constexpr` extensively for compile-time computation
- Concepts for template constraints
- Designated initializers for clarity
- `std::ranges` for algorithms
- `[[nodiscard]]` on functions returning values that must be checked

### Error Handling Strategy
```cpp
// Use std::expected for fallible operations
auto Pager::read_page(PageId id) -> std::expected<Page, Error>;

// Error type
enum class ErrorCode {
    IoError,
    CorruptDatabase,
    OutOfMemory,
    // ...
};

struct Error {
    ErrorCode code;
    std::string message;
    std::source_location location;
};
```

### Naming Conventions
- Types: `PascalCase` (e.g., `PageCache`, `BTreeCursor`)
- Functions/methods: `snake_case` (e.g., `read_page`, `find_cell`)
- Constants: `kPascalCase` (e.g., `kPageSize`, `kMaxCachePages`)
- Member variables: `snake_case_` with trailing underscore
- Namespaces: `snake_case` (e.g., `sqlite::btree`)

### File Naming
- Headers: `.hpp` extension
- Implementation: `.cpp` extension
- Test files: `*_test.cpp`
- Benchmark files: `*_bench.cpp`

---

## Testing Conventions

### Framework
Google Test (gtest) with Google Mock (gmock)

### Test Organization
```
tests/
├── unit/           # Unit tests (isolated, mocked dependencies)
│   ├── pager/
│   ├── btree/
│   └── sql/
├── integration/    # Integration tests (real components together)
└── fixtures/       # Shared test utilities and data
```

### Test Naming
```cpp
TEST(BTreeCursor, FindsExistingKeyInLeafNode) { ... }
TEST(BTreeCursor, ReturnsNotFoundForMissingKey) { ... }
TEST(Pager, WritesPageToDiskOnEviction) { ... }
```

### Test Coverage Targets
- Unit tests: 80%+ line coverage
- All public APIs must have tests
- All error paths must be tested

---

## Git Workflow

### Branch Strategy
- `main` - Always deployable, protected
- `feature/*` - Feature branches
- `fix/*` - Bug fix branches
- `docs/*` - Documentation updates

### Commit Messages
Follow Conventional Commits:
```
type(scope): subject

body

footer
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `build`, `ci`

### Pull Request Process
1. Create feature branch from `main`
2. Implement with tests
3. Ensure CI passes (build, test, sanitizers)
4. Request review
5. Squash merge to `main`

---

## Implementation Roadmap

### Milestone 1: Foundation
- Project setup (CMake, Conan, CI)
- OS abstraction layer (file I/O)
- Page structure and serialization
- Basic pager (read/write pages)

### Milestone 2: B-tree Storage
- B-tree page format
- B-tree cursor (navigation)
- B-tree insert/delete
- B-tree split/merge

### Milestone 3: SQL Frontend
- Tokenizer
- Parser (recursive descent)
- AST representation
- Basic semantic analysis

### Milestone 4: Virtual Machine
- Instruction set design
- VM execution loop
- Code generator
- Cursor integration

### Milestone 5: Integration
- Database class (public API)
- Schema management
- Transaction support
- Query result handling
