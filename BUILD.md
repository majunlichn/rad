# How to build

## Prerequisites

- Git, CMake 3.25+, C++ compiler (MSVC, Clang, or GCC)
- Python 3 (for `setup.py` to install third-party dependencies)
- [vcpkg](https://github.com/microsoft/vcpkg) and environment variable `VCPKG_ROOT`
- [Vulkan SDK](https://vulkan.lunarg.com/) and environment variable `VULKAN_SDK`

## Build

```bash
python setup.py
cmake -S . -B build
cmake --build build
```

## CMake options

| Option | Default | Requires |
|--------|---------|----------|
| `RAD_BUILD_VULKAN` | ON | Vulkan SDK, `VULKAN_SDK` |
| `RAD_BUILD_GUI` | ON | SDL3 |
| `RAD_BUILD_MULTIMEDIA` | ON | SDL3, SDL3_mixer |
| `RAD_BUILD_TESTS` | ON | GTest (vcpkg) |
| `RAD_ENABLE_INSTALL` | ON | — |

## Third-party dependencies

You can set package search paths to use custom builds (cache or environment):

| Variable | Description |
|----------|---------|
| `SDL3_DIR` | Directory containing `SDL3Config.cmake` |
| `SDL3_mixer_DIR` | Directory containing `SDL3_mixerConfig.cmake` |

If no path is set, CMake checks `external/` for libraries installed by `setup.py`, then falls back to the default search paths.

On Linux vcpkg sdl3 and sdl3-mixer may not work properly.

### Using setup.py

Run **`setup.py`** from the repo root to clone and build dependencies under `external/`:

| Library | Task | Needed for |
|---------|------|------------|
| SDL3 | `sdl3` | `RAD_BUILD_GUI` |
| SDL3_mixer | `sdl3-mixer` | `RAD_BUILD_MULTIMEDIA` |

Each library uses `external/<folder>/` with subdirectories `source`, `build`, and `installed` (`SDL` for SDL3, `SDL_mixer` for SDL3_mixer).

Dependencies are resolved automatically (e.g. `--only sdl3-mixer` also builds `sdl3` if its CMake package is not already present). A library is skipped when its install prefix already contains a valid `*Config.cmake`; use `--force` to rebuild.

`--force` applies only to libraries named with `--only` (not auto-pulled dependencies). It clears the build tree before configure, compiles, then clears the install prefix before reinstalling (keeps `source/`). If the compile step fails, the previous install is left in place.

Parallelism defaults to the CPU count (`-j` / `--jobs`); the value is capped at that same limit. Generator and platform can be set with `-G` / `-A`, or via `CMAKE_GENERATOR` and `CMAKE_GENERATOR_PLATFORM` when those flags are omitted.

To re-clone from scratch, delete the library folder under `external/` manually.

```bash
# Install all (sdl3, then sdl3-mixer)
python setup.py
# Install only SDL3
python setup.py --only sdl3
# SDL3_mixer (builds sdl3 first if needed)
python setup.py --only sdl3-mixer
# Force rebuild SDL3 only (does not force-rebuild sdl3-mixer)
python setup.py --only sdl3 --force
# Force rebuild SDL3_mixer only (reuses existing SDL3 install)
python setup.py --only sdl3-mixer --force
# Specify CMake generator, architecture
python setup.py -G "Visual Studio 18 2026" -A x64
# Limit parallel build jobs
python setup.py -j 8
```
