cmake_minimum_required(VERSION 3.25)

# Setup vcpkg toolchain file automatically.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE FILEPATH "vcpkg toolchain file" FORCE)
endif()
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    cmake_path(NORMAL_PATH CMAKE_TOOLCHAIN_FILE)
endif()
message(STATUS "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")

project(rad VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(RAD_BUILD_VULKAN "Build the Vulkan library" ON)
option(RAD_BUILD_GUI "Build the GUI library" ON)
option(RAD_BUILD_MULTIMEDIA "Build the Multimedia library" ON)
option(RAD_BUILD_TESTS "Build tests" ON)
option(RAD_ENABLE_INSTALL "Enable installation rules" ON)

include(cmake/radCompileOptions.cmake)
include(cmake/radSanitizers.cmake)

set(RAD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

get_property(IS_MULTICONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# Set output directories
if(IS_MULTICONFIG_GENERATOR)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
else()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

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

find_package(Vulkan QUIET)
message(STATUS "Vulkan_FOUND=${Vulkan_FOUND}")

if(RAD_BUILD_VULKAN)
    if(NOT Vulkan_FOUND)
        message(FATAL_ERROR
            "RAD_BUILD_VULKAN is ON but Vulkan SDK was not found.\n"
            "Install the Vulkan SDK and set the VULKAN_SDK environment variable.")
    endif()
    add_subdirectory(src/rad/Vulkan)
endif()

if(RAD_BUILD_GUI OR RAD_BUILD_MULTIMEDIA)
    include(cmake/FindSDL3.cmake)
    sdl3_resolve()
endif()

if(RAD_BUILD_GUI)
    add_subdirectory(src/rad/Gui)
endif()

if(RAD_BUILD_MULTIMEDIA)
    sdl3_mixer_resolve()
    add_subdirectory(src/rad/Multimedia)
endif()

# https://cmake.org/cmake/help/latest/prop_gbl/USE_FOLDERS.html
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
