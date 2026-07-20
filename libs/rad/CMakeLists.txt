set(RAD_SOURCES
    Core/Float.h
    Core/Float.cpp
    Core/Integer.h
    Core/IntegerFallback.h
    Core/Integer.cpp
    Core/Platform.h
    Core/Platform.cpp
)

set(RAD_TEST_SOURCES
    test.cpp
    Core/Float.test.cpp
    Core/Integer.test.cpp
    Core/Platform.test.cpp
)

add_library(rad STATIC)
add_library(rad::rad ALIAS rad)

target_sources(rad PRIVATE ${RAD_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SOURCES})

target_include_directories(rad
    PUBLIC
        $<BUILD_INTERFACE:${RAD_PROJECT_ROOT}/libs>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
