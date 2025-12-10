FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ccache \
    cmake \
    libdecor-0-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libpthread-stubs0-dev \
    libwayland-dev \
    libx11-dev \
    libxcursor-dev \
    libxi-dev \
    libxinerama-dev \
    libxkbcommon-dev \
    libxrandr-dev \
    libxxf86vm-dev \
    mold \
    ninja-build \
    pkg-config \
    wayland-protocols \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY CMakeLists.txt ./
COPY external/ external/
COPY include/ include/
COPY src/ src/
COPY assets/ assets/
COPY tests/ tests/

RUN mkdir -p cmake-build && cd cmake-build && \
    cmake -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
        .. && \
    cmake --build . --parallel $(nproc)

RUN mkdir -p cmake-build-tests && cd cmake-build-tests && \
    cmake -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_TESTS=ON \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        -DCMAKE_EXE_LINKER_FLAGS="-fuse-ld=mold" \
        .. && \
    cmake --build . --parallel $(nproc)

RUN test -f /build/cmake-build/scenelab && echo "Build successful!" && \
    test -f /build/cmake-build-tests/scenelab_tests && echo "Tests build successful!"

FROM ubuntu:24.04 AS test-runner

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=builder /build/cmake-build-tests/scenelab_tests ./scenelab_tests

COPY --from=builder /build/cmake-build/scenelab ./scenelab

CMD ["./scenelab_tests", "--gtest_color=yes"]
