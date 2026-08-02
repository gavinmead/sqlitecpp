# Linux development container

SQLiteCpp targets **Linux only**. To develop on macOS (Apple Silicon) with a fast, debuggable
workflow, all building and debugging happens inside an Ubuntu 24.04 container that reproduces the CI
toolchain: **GCC 14**, **Clang 18**, C++23, Conan 2, Ninja, plus `gdb` and `ccache`.

The image is built native `linux/arm64` on Apple Silicon, so builds run at full speed (no qemu) and
`gdb` runs natively inside the container against the Linux binary.

One `.devcontainer/Dockerfile` serves three consumers: the CLion Docker toolchain, CLion's native
Dev Containers, and VS Code Dev Containers.

## Prerequisites

- Docker Desktop running.
- CLion (2023.2+ for Docker toolchains; 2024.2+ for native Dev Containers).

---

## Path A — CLion Docker toolchain (recommended)

The simplest and most stable path for debugging. CLion runs CMake, the build, and `gdb` inside
ephemeral containers based on the image; your source is bind-mounted from the host.

1. **Build the image:**
   ```bash
   make docker-image      # docker build -t sqlitecpp-dev:latest .devcontainer
   ```
2. **Add the toolchain:** Settings → Build, Execution, Deployment → **Toolchains → + → Docker**.
   Set **Image** to `sqlitecpp-dev:latest`. CLion auto-detects CMake, Ninja, the C/C++ compilers,
   and **gdb** inside the image.
3. **Add a CMake profile:** Settings → **CMake → +**. Select the Docker toolchain and the
   **`dev-debug`** preset (Build type Debug). It configures into `build/dev-debug`.
4. **Build & Debug:** set a breakpoint (e.g. in an `os` unit test) and run **Debug**. It stops at the
   breakpoint because `gdb` runs in the container against the Linux binary. Because `.clangd` already
   points at `build/dev-debug`, code insight works too.

Use `dev-release` for a Release profile. The `ci-clang-*` presets build with Clang 18 instead of the
default GCC 14.

## Path B — CLion native Dev Containers / VS Code

Runs a full IDE backend inside the container, reading `.devcontainer/devcontainer.json`.

- **CLion:** open the project → *Dev Containers* → build & connect using `.devcontainer/devcontainer.json`.
- **VS Code:** with the Dev Containers extension, "Reopen in Container".

---

## Notes

- **First configure** triggers `cmake/conan_provider.cmake` (cmake-conan) to fetch/build gtest into
  the Conan cache. That cache and ccache live in named Docker volumes (`sqlitecpp-conan`,
  `sqlitecpp-ccache`) so they stay warm across container recreates — later configures are fast.
- **Quick shell** in the toolchain image without CLion:
  ```bash
  make docker-shell      # docker run --rm -it -v "$PWD":/work -w /work sqlitecpp-dev:latest bash
  make test              # inside the container
  ```
- **Architecture:** the image is `linux/arm64` on Apple Silicon. CI runs `linux/amd64`; this is fine
  because the project has no architecture-specific code.
- **Passwordless sudo:** the `ubuntu` user has `NOPASSWD:ALL` sudo. That is deliberate for a local,
  throwaway dev container (installing extra tooling, chowning mounted volumes). Do **not** copy this
  pattern into any CI or production-adjacent image.
