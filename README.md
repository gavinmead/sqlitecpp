# sqlitecpp

An educational port of SQLite using modern C++23.

## Development

SQLiteCpp targets **Linux only**. On macOS, development runs inside an Ubuntu 24.04 container that
reproduces the CI toolchain (GCC 14, Clang 18, C++23, Conan 2, Ninja) with `gdb` for debugging. On
Apple Silicon the image builds native `linux/arm64`, so builds and debugging run at full speed.

Quick start:

```bash
make docker-image          # build the dev container image (sqlitecpp-dev:latest)
make docker-shell          # shell into it with the repo mounted at /work
make test                  # inside the container: configure, build, run tests
```

For CLion (Docker toolchain and native Dev Containers) and VS Code setup, plus debugging
instructions, see [docs/DEVCONTAINER.md](docs/DEVCONTAINER.md).
