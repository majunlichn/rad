# How to build

## Prerequisites

Install the following tools:

- Git
- CMake (3.25+)
- A C++ compiler (MSVC, Clang, or GCC)

Set up [vcpkg](https://github.com/microsoft/vcpkg).

For convenience, set the `VCPKG_ROOT` environment variable to your vcpkg directory. This project will automatically use the vcpkg CMake toolchain file when `VCPKG_ROOT` is set.

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Options

You can control what gets built by passing CMake cache options at configure time:

```bash
cmake -S . -B build -DRAD_BUILD_GUI=ON -DRAD_BUILD_VULKAN=ON -DRAD_BUILD_TESTS=ON
```

- **`RAD_BUILD_VULKAN`**: Build the Vulkan library (default: `ON`, requires VulkanSDK and environment variable `VULKAN_SDK` set).
- **`RAD_BUILD_GUI`**: Build the GUI library (default: `ON`, requires the SDL library).
- **`RAD_BUILD_TESTS`**: Build tests (default: `ON`).
- **`RAD_ENABLE_INSTALL`**: Enable installation rules (default: `ON`).
- **`SDL3_INSTALL_DIR`**: Optional SDL3 install prefix to use instead of the default SDL3 discovery.

### Linux

On Linux, SDL3 from vcpkg may not work properly. If you run into issues, build SDL3 from source and point CMake at that install.

```bash
git clone --depth=1 https://github.com/libsdl-org/SDL.git
cd SDL
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=installed
cmake --build build --target install
```

Then configure `rad` with `SDL3_INSTALL_DIR`:

```bash
cmake -S . -B build -DSDL3_INSTALL_DIR=/path/to/SDL/installed
cmake --build build
```
