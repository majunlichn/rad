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
cmake -S . -B build -DRAD_BUILD_GUI=ON -DRAD_BUILD_MULTIMEDIA=ON -DRAD_BUILD_VULKAN=ON -DRAD_BUILD_TESTS=ON
```

- **`RAD_BUILD_VULKAN`**: Build the Vulkan library (default: `ON`, requires VulkanSDK and environment variable `VULKAN_SDK` set).
- **`RAD_BUILD_GUI`**: Build the GUI library (default: `ON`, requires SDL3).
- **`RAD_BUILD_MULTIMEDIA`**: Build the Multimedia library (default: `ON`, requires SDL3 and SDL3_mixer).
- **`RAD_BUILD_TESTS`**: Build tests (default: `ON`).
- **`RAD_ENABLE_INSTALL`**: Enable installation rules (default: `ON`).

### SDL3 and SDL3_mixer

When `RAD_BUILD_GUI` or `RAD_BUILD_MULTIMEDIA` is enabled, the top-level `CMakeLists.txt` includes `cmake/FindSDL3.cmake` and calls **`sdl3_resolve()`**. When `RAD_BUILD_MULTIMEDIA` is on, it also calls **`sdl3_mixer_resolve()`** before adding the Multimedia target.

Resolution order for each library:

1. **Explicit path**: CMake cache or environment **`SDL3_DIR`** / **`SDL3_mixer_DIR`** (directory that contains `SDL3Config.cmake` or `SDL3_mixerConfig.cmake`).
2. **`find_package(... CONFIG)`** on the normal search path (for example vcpkg when the toolchain is active).
3. **FetchContent** from GitHub if the package is still not found. Tags default to stable releases and can be overridden:
   - **`SDL3_GIT_TAG`** (default `release-3.4.8`)
   - **`SDL3_MIXER_GIT_TAG`** (default `release-3.2.2`; SDL3_mixer is cloned with recursive submodules).

Those functions are **idempotent**: if `SDL3::SDL3` or `SDL3_mixer::SDL3_mixer` is already defined, a later call does nothing. They are not CMake’s built-in `find_package(SDL3)` find-module; the filename `FindSDL3.cmake` is included explicitly.

If SDL is provided by vcpkg, it is usually picked up in step 2 and FetchContent does not run. To prefer a source build instead, remove SDL from the manifest or point **`SDL3_DIR`** at your install’s `lib/cmake/SDL3` (and the same idea for mixer).

### Linux

On Linux, SDL3 from vcpkg may not work properly. If you run into issues, build SDL3 from source and point CMake at the CMake package directory.

```bash
git clone --depth=1 https://github.com/libsdl-org/SDL.git
cd SDL
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=installed
cmake --build build --target install
```

Then configure `rad` with `SDL3_DIR` (adjust the path to match your install layout):

```bash
cmake -S . -B build -DSDL3_DIR=/path/to/SDL/installed/lib/cmake/SDL3
cmake --build build
```

For SDL3_mixer, use **`SDL3_mixer_DIR`** the same way (directory containing `SDL3_mixerConfig.cmake`), or rely on vcpkg / FetchContent when it is not set.
