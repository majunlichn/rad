# Build

## Prerequisite

- Setup vcpkg
    - Clone: https://github.com/microsoft/vcpkg
    - Run script `bootstrap-vcpkg.bat` or `bootstrap-vcpkg.sh` in vcpkg.
    - Set environment variable `VCPKG_ROOT` to the root directory of vcpkg.

## Windows MSVC

Build default:
```
cmake -S . -B build
```

Build with AddressSanitizer:
```
cmake -S . -B build-asan -DENABLE_ASAN=ON -DVCPKG_OVERLAY_TRIPLETS=triplets -DVCPKG_TARGET_TRIPLET=x64-windows-asan
```
