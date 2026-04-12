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
option(RAD_BUILD_VULKAN "Build Vulkan support" ON)
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
    src/rad/Common/BFloat16.h
    src/rad/Common/Flags.h
    src/rad/Common/Float.h
    src/rad/Common/Float16.h
    src/rad/Common/Float8.h
    src/rad/Common/Hash.h
    src/rad/Common/Integer.h
    src/rad/Common/Math.h
    src/rad/Common/Memory.h
    src/rad/Common/MurmurHash3.h
    src/rad/Common/Platform.h
    src/rad/Common/Random.h
    src/rad/Common/Range.h
    src/rad/Common/RefCounted.h
    src/rad/Common/String.h
    src/rad/Common/TypeTraits.h
    src/rad/Container/SmallVector.h
    src/rad/Container/Span.h
    src/rad/IO/File.h
    src/rad/IO/Image.h
    src/rad/IO/Logging.h
    src/rad/System/Application.h
    src/rad/System/CpuInfo.h
    src/rad/System/MemoryDebug.h
    src/rad/System/OS.h
    src/rad/System/Process.h
    src/rad/System/StackTrace.h
    src/rad/System/Thread.h
    src/rad/System/Time.h
)

set(RAD_SOURCES
    src/rad/Common/Float.cpp
    src/rad/Common/Hash.cpp
    src/rad/Common/Integer.cpp
    src/rad/Common/Math.cpp
    src/rad/Common/Memory.cpp
    src/rad/Common/MurmurHash3.cpp
    src/rad/Common/Platform.cpp
    src/rad/Common/Random.cpp
    src/rad/Common/String.cpp
    src/rad/IO/File.cpp
    src/rad/IO/Image.cpp
    src/rad/IO/Logging.cpp
    src/rad/System/Application.cpp
    src/rad/System/CpuInfo.cpp
    src/rad/System/MemoryDebug.cpp
    src/rad/System/OS.cpp
    src/rad/System/Process.cpp
    src/rad/System/StackTrace.cpp
    src/rad/System/Thread.cpp
    src/rad/System/Time.cpp
)

set(RAD_TEST_SOURCES
    src/rad/Common/Float.test.cpp
    src/rad/Common/Integer.test.cpp
    src/rad/Common/Math.test.cpp
    src/rad/Common/Platform.test.cpp
    src/rad/Common/Random.test.cpp
    src/rad/Common/Range.test.cpp
    src/rad/Common/String.test.cpp
    src/rad/Container/Span.test.cpp
    src/rad/IO/File.test.cpp
    src/rad/IO/Image.test.cpp
    src/rad/System/Process.test.cpp
    src/rad/System/StackTrace.test.cpp
    src/rad/System/Time.test.cpp
    src/rad/unit_tests.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/rad
    FILES ${RAD_HEADERS} ${RAD_SOURCES} ${RAD_TEST_SOURCES}
)

add_library(rad STATIC)

target_sources(rad PRIVATE ${RAD_HEADERS} ${RAD_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/rad FILES ${RAD_HEADERS} ${RAD_SOURCES})

target_include_directories(rad
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imported/half/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imported/pcg-cpp/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(Backward CONFIG REQUIRED)
find_package(boost_asio REQUIRED CONFIG)
find_package(boost_container REQUIRED CONFIG)
find_package(boost_filesystem REQUIRED CONFIG)
find_package(boost_locale CONFIG REQUIRED)
find_package(boost_nowide CONFIG REQUIRED)
find_package(boost_process REQUIRED CONFIG)
find_package(boost_smart_ptr CONFIG REQUIRED)
find_package(CpuFeatures CONFIG REQUIRED)
find_package(pystring CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(xxHash CONFIG REQUIRED)

add_subdirectory(imported/cstring_view)

target_include_directories(rad
    PRIVATE ${Stb_INCLUDE_DIR}
)

target_link_libraries(rad
    PUBLIC Backward::Backward
    PUBLIC Boost::asio
    PUBLIC Boost::container
    PUBLIC Boost::filesystem
    PUBLIC Boost::locale
    PUBLIC Boost::nowide
    PUBLIC Boost::process
    PUBLIC Boost::smart_ptr
    PUBLIC beman::cstring_view
    PUBLIC CpuFeatures::cpu_features
    PUBLIC pystring::pystring
    PUBLIC range-v3::range-v3
    PUBLIC spdlog::spdlog
    PUBLIC xxHash::xxhash
)

target_compile_features(rad PUBLIC cxx_std_20)

if (WIN32)
    target_compile_definitions(rad
        PUBLIC NOMINMAX=1
        PUBLIC WIN32_LEAN_AND_MEAN=1
        PUBLIC _WIN32_WINNT=0x0A00
    )
    target_compile_options(rad
        PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    )
endif()

set_target_properties(rad PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

### Tests ##########################################################################################

if(RAD_BUILD_TESTS)
    enable_testing()

    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)

    add_executable(rad_unit_tests)
    target_sources(rad_unit_tests PRIVATE ${RAD_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src/rad FILES ${RAD_TEST_SOURCES})
    target_include_directories(rad_unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(rad_unit_tests
        PRIVATE rad
        PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
    )
    set_target_properties(rad_unit_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        PROPERTY VS_STARTUP_PROJECT rad_unit_tests
    )
    gtest_discover_tests(rad_unit_tests)
endif()

### Optional Features ##############################################################################

if (RAD_BUILD_VULKAN AND DEFINED ENV{VULKAN_SDK})
    add_subdirectory(src/rad/Vulkan)
endif()

### Installation ###################################################################################

if(RAD_ENABLE_INSTALL)

include(GNUInstallDirs)

install(TARGETS rad
    EXPORT radTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/rad/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rad
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl"
)

install(EXPORT radTargets
    FILE radTargets.cmake
    NAMESPACE rad::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/rad
)

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
