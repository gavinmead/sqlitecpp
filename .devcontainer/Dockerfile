# SQLiteCpp development container.
#
# Reproduces the CI toolchain (Ubuntu 24.04, GCC 14, Clang 18, C++23, Conan 2,
# Ninja) plus a debugger, so CLion / VS Code can build and debug Linux binaries
# from an Apple Silicon Mac at native arm64 speed.
#
# Consumed by:
#   - CLion Docker toolchain      (docker build -t sqlitecpp-dev:latest .devcontainer)
#   - CLion native Dev Containers (.devcontainer/devcontainer.json)
#   - VS Code Dev Containers      (.devcontainer/devcontainer.json)
#
# The base tag is intentionally unpinned (no @sha256 digest): this is a
# dev-only image, and CI itself provisions on the mutable `ubuntu-24.04`
# runner with unpinned apt packages. Pinning a digest here would *diverge*
# from CI rather than match it. Accept the drift; the pinned versions that
# matter (GCC 14, Clang 18) are named explicitly below.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

# Toolchain matching CI (GCC 14 + Clang 18) plus build & debug tooling.
# clang-18 and friends live in Ubuntu 24.04's "universe" component, which is
# enabled in the official ubuntu:24.04 image. If a future base image drops it,
# fall back to the apt.llvm.org/llvm.sh 18 script that CI uses.
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        gcc-14 g++-14 \
        clang-18 clang-tidy-18 clang-format-18 lld-18 \
        libc++-18-dev libc++abi-18-dev \
        cmake ninja-build \
        gdb \
        ccache \
        git wget ca-certificates sudo \
        python3 python3-pip python3-venv \
    && rm -rf /var/lib/apt/lists/*

# Make the pinned compiler versions the unqualified defaults. This lets the
# ci-clang-* presets (which set CMAKE_CXX_COMPILER=clang++) and the Makefile
# format/lint targets resolve to clang-18, and gcc/g++ to 14.
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 \
        --slave /usr/bin/g++ g++ /usr/bin/g++-14 \
    && update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 100 \
        --slave /usr/bin/clang++ clang++ /usr/bin/clang++-18 \
        --slave /usr/bin/clang-format clang-format /usr/bin/clang-format-18 \
        --slave /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18

# Conan + gcovr. Ubuntu 24.04's system Python is PEP-668 "externally managed",
# so --break-system-packages is required for a global install in the container.
RUN pip3 install --break-system-packages --no-cache-dir conan gcovr

# Deterministic default compiler for the dev-* presets, plus ccache to make
# incremental rebuilds fast.
ENV CC=gcc-14 \
    CXX=g++-14 \
    CMAKE_CXX_COMPILER_LAUNCHER=ccache

# Reuse the image's existing uid-1000 "ubuntu" user (keeps bind-mounted file
# ownership sane on the host) and give it passwordless sudo.
RUN echo 'ubuntu ALL=(ALL) NOPASSWD:ALL' > /etc/sudoers.d/ubuntu \
    && chmod 0440 /etc/sudoers.d/ubuntu

USER ubuntu
WORKDIR /home/ubuntu

# Seed a default Conan profile (detects gcc-14 / libstdc++11) for the ubuntu user.
# This baked-in profile is what the non-volume consumers use: `make docker-shell`,
# `docker run`, and the CLion Docker toolchain, none of which mount a Conan volume.
# Under devcontainer.json the named `sqlitecpp-conan` volume mounts over ~/.conan2
# and shadows this profile, so postCreateCommand re-runs detect there.
RUN conan profile detect --force
