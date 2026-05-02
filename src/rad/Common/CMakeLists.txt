set(RAD_COMMON_HEADERS
    BFloat16.h
    Flags.h
    Float.h
    Float16.h
    Float8.h
    Hash.h
    Integer.h
    Math.h
    Memory.h
    MemoryDebug.h
    Platform.h
    Random.h
    Range.h
    RefCounted.h
    Result.h
    String.h
    TypeTraits.h
    UTFConv.h
    imported/smhasher/src/MurmurHash3.h
)

set(RAD_COMMON_SOURCES
    Float.cpp
    Hash.cpp
    Integer.cpp
    Math.cpp
    Memory.cpp
    MemoryDebug.cpp
    Platform.cpp
    Random.cpp
    String.cpp
    UTFConv.cpp
    imported/smhasher/src/MurmurHash3.cpp
)

set(RAD_COMMON_TEST_SOURCES
    Common.test.h
    Common.test.cpp
    Float.test.cpp
    Integer.test.cpp
    Math.test.cpp
    UTFConv.test.cpp
)

add_library(rad_common STATIC)

target_sources(rad_common PRIVATE ${RAD_COMMON_HEADERS} ${RAD_COMMON_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_COMMON_HEADERS} ${RAD_COMMON_SOURCES})

target_include_directories(rad_common
    PUBLIC
        $<BUILD_INTERFACE:${RAD_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imported/half/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imported/pcg-cpp/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imported/smhasher/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imported/cstring_view/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(boost_locale CONFIG REQUIRED)
find_package(boost_outcome CONFIG REQUIRED)
find_package(pystring CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)

target_link_libraries(rad_common
    PUBLIC Boost::locale
    PUBLIC Boost::outcome
    PUBLIC pystring::pystring
    PUBLIC range-v3::range-v3
    PUBLIC xxHash::xxhash
)

if (WIN32)
    target_compile_definitions(rad_common
        PUBLIC NOMINMAX=1
        PUBLIC WIN32_LEAN_AND_MEAN=1
        PUBLIC _WIN32_WINNT=0x0A00
    )
    target_compile_options(rad_common
        PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    )
endif()

if(RAD_BUILD_TESTS)
    add_executable(rad_common_tests)
    target_sources(rad_common_tests PRIVATE ${RAD_COMMON_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_COMMON_TEST_SOURCES})
    target_link_libraries(rad_common_tests
        PRIVATE rad_common
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_common_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_common_tests)
endif()
