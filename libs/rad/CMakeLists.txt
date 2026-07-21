set(RAD_SOURCES
    Core/BFloat16.h
    Core/BFloat16.cpp
    Core/Float.h
    Core/Float.cpp
    Core/Float16.h
    Core/Float16.cpp
    Core/Float8.h
    Core/Float8.cpp
    Core/Hash.h
    Core/Hash.cpp
    Core/Integer.h
    Core/IntegerFallback.h
    Core/Integer.cpp
    Core/Memory.h
    Core/Memory.cpp
    Core/MurmurHash3.h
    Core/MurmurHash3.cpp
    Core/Platform.h
    Core/Platform.cpp
    Core/RefCounted.h
    Core/Span.h
    Core/String.h
    Core/String.cpp
    Core/TypeTraits.h
)

set(RAD_TEST_SOURCES
    test.cpp
    Core/BFloat16.test.cpp
    Core/Float.test.cpp
    Core/Float8.test.cpp
    Core/Integer.test.cpp
    Core/Memory.test.cpp
    Core/Platform.test.cpp
    Core/Span.test.cpp
    Core/String.test.cpp
)

add_library(rad STATIC)
add_library(rad::rad ALIAS rad)

target_sources(rad PRIVATE ${RAD_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SOURCES})

find_package(Imath CONFIG REQUIRED)
find_package(boost_algorithm CONFIG REQUIRED)
find_package(boost_smart_ptr CONFIG REQUIRED)
find_package(pystring CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)

target_include_directories(rad
    PUBLIC
        $<BUILD_INTERFACE:${RAD_PROJECT_ROOT}/libs>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(rad
    PRIVATE Boost::algorithm
    PUBLIC Boost::smart_ptr
    PRIVATE pystring::pystring
    PUBLIC Imath::Imath
    PUBLIC xxHash::xxhash
)

if(WIN32)
    target_compile_definitions(rad
        PUBLIC NOMINMAX=1
        PUBLIC WIN32_LEAN_AND_MEAN=1
        PUBLIC _WIN32_WINNT=0x0A00
    )
    target_compile_options(rad
        PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
    )
endif()

if(RAD_BUILD_TESTS)
    add_executable(rad_tests)
    target_sources(rad_tests PRIVATE ${RAD_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_TEST_SOURCES})
    target_link_libraries(rad_tests
        PRIVATE rad
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_tests)
endif()
