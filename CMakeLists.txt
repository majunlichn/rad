cmake_minimum_required(VERSION 3.20)

# Setup vcpkg toolchain file automatically.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE FILEPATH "vcpkg toolchain file" FORCE)
endif()
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    cmake_path(NORMAL_PATH CMAKE_TOOLCHAIN_FILE)
    list(REMOVE_DUPLICATES CMAKE_TOOLCHAIN_FILE)
endif()
message(STATUS "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")

project(rad VERSION 0.0.0 LANGUAGES CXX)

# Build options
option(RAD_BUILD_VULKAN "Build the Vulkan library" ON)
option(RAD_BUILD_GUI "Build the GUI library" ON)
option(RAD_BUILD_TESTS "Build tests" ON)
option(RAD_ENABLE_INSTALL "Enable installation rules" ON)

include(cmake/radCompileOptions.cmake)
include(cmake/radSanitizers.cmake)

set(RAD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set default install prefix (if not specified by user)
if(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_BINARY_DIR}/install")
  message(STATUS "Setting default install prefix: ${CMAKE_INSTALL_PREFIX}")
endif()

if(RAD_BUILD_TESTS)
    enable_testing()
    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)
endif()

add_subdirectory(src/rad/Common)
add_subdirectory(src/rad/Container)
add_subdirectory(src/rad/IO)
add_subdirectory(src/rad/System)
if (RAD_BUILD_VULKAN AND DEFINED ENV{VULKAN_SDK})
    add_subdirectory(src/rad/Vulkan)
endif()
if (RAD_BUILD_GUI)
    add_subdirectory(src/rad/Gui)
endif()
