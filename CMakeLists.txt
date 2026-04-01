cmake_minimum_required(VERSION 3.20)

# Setup vcpkg toolchain file automatically.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE FILEPATH "vcpkg toolchain file" FORCE)
endif()
if(DEFINED CMAKE_TOOLCHAIN_FILE)
    cmake_path(CONVERT ${CMAKE_TOOLCHAIN_FILE} TO_CMAKE_PATH_LIST CMAKE_TOOLCHAIN_FILE NORMALIZE)
    list(REMOVE_DUPLICATES CMAKE_TOOLCHAIN_FILE)
endif()
message(STATUS "CMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}")

project(rad VERSION 0.0.0 LANGUAGES CXX)

# Build options
option(RAD_BUILD_TESTS "Build unit tests" ON)
option(RAD_ENABLE_INSTALL "Enable installation rules" ON)

# C++ standard configuration
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set default install prefix (if not specified by user)
if(WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set_property(CACHE CMAKE_INSTALL_PREFIX PROPERTY VALUE "${CMAKE_BINARY_DIR}/install")
  message(STATUS "Setting default install prefix: ${CMAKE_INSTALL_PREFIX}")
endif()

set(RAD_HEADERS
    rad/Common/Platform.h
    rad/Common/Integer.h
)

set(RAD_SOURCES
    rad/Common/Platform.cpp
    rad/Common/Integer.cpp
)

set(RAD_TEST_SOURCES
    rad/Common/Platform.test.cpp
    rad/Common/Integer.test.cpp
    rad/tests.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${RAD_HEADERS} ${RAD_SOURCES} ${RAD_TEST_SOURCES}
)

add_library(rad STATIC)

target_sources(rad
    PRIVATE
        ${RAD_HEADERS}
        ${RAD_SOURCES}
)

target_include_directories(rad
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(rad PUBLIC cxx_std_20)

if (WIN32)
    target_compile_definitions(rad
        PRIVATE NOMINMAX=1
        PRIVATE WIN32_LEAN_AND_MEAN=1
    )
    target_compile_options(rad
        PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    )
endif()

set_target_properties(rad PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

### Tests #############################################################################################################

if(RAD_BUILD_TESTS)
    enable_testing()

    find_package(GTest CONFIG REQUIRED)

    add_executable(rad_tests)
    target_sources(rad_tests PRIVATE
        ${RAD_TEST_SOURCES}
    )
    target_include_directories(rad_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(rad_tests
        PRIVATE rad
        PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    )
    set_target_properties(rad_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "$<IF:$<CONFIG:Debug>,${CMAKE_BINARY_DIR}/bin/Debug,${CMAKE_BINARY_DIR}/bin/Release>"
    )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT rad_tests
    )
    add_test(NAME rad_tests COMMAND rad_tests)
endif()

### Installation ######################################################################################################

if(RAD_ENABLE_INSTALL)

include(GNUInstallDirs)

install(TARGETS rad
    EXPORT radTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/rad/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rad
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

install(EXPORT radTargets
    FILE radTargets.cmake
    NAMESPACE rad::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rad
)

if(RAD_BUILD_TESTS)
    install(TARGETS rad_tests
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
    install(FILES $<TARGET_RUNTIME_DLLS:rad_tests>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        OPTIONAL
    )
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/radConfig.cmake.in
    radConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rad
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file(
    radConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/radConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/radConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rad
)

endif() # RAD_ENABLE_INSTALL
