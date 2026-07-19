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

set(RAD_PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(RAD_BUILD_TESTS "Build tests" ON)
option(RAD_ENABLE_INSTALL "Enable installation rules" ON)

include(GNUInstallDirs)
include(cmake/radCompileOptions.cmake)
include(cmake/radSanitizers.cmake)

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

add_subdirectory(libs/rad)

if(RAD_ENABLE_INSTALL)
    include(CMakePackageConfigHelpers)

    install(
        TARGETS rad
        EXPORT radTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(
        DIRECTORY "${RAD_PROJECT_ROOT}/libs/rad/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/rad"
        FILES_MATCHING
            PATTERN "*.h"
    )

    set(RAD_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/rad")

    install(
        EXPORT radTargets
        NAMESPACE rad::
        DESTINATION "${RAD_INSTALL_CMAKEDIR}"
    )

    configure_package_config_file(
        cmake/radConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/radConfig.cmake"
        INSTALL_DESTINATION "${RAD_INSTALL_CMAKEDIR}"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/radConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/radConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/radConfigVersion.cmake"
        DESTINATION "${RAD_INSTALL_CMAKEDIR}"
    )
endif()

# https://cmake.org/cmake/help/latest/prop_gbl/USE_FOLDERS.html
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
