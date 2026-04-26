set(RAD_SYSTEM_HEADERS
    Application.h
    CpuInfo.h
    OS.h
    Process.h
    StackTrace.h
    Thread.h
    Time.h
)

set(RAD_SYSTEM_SOURCES
    Application.cpp
    CpuInfo.cpp
    OS.cpp
    Process.cpp
    StackTrace.cpp
    Thread.cpp
    Time.cpp
)

set(RAD_SYSTEM_TEST_SOURCES
    System.test.h
    System.test.cpp
    Time.test.cpp
)

add_library(rad_system STATIC)

target_sources(rad_system PRIVATE ${RAD_SYSTEM_HEADERS} ${RAD_SYSTEM_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SYSTEM_HEADERS} ${RAD_SYSTEM_SOURCES})

find_package(Backward CONFIG REQUIRED)
find_package(boost_asio REQUIRED CONFIG)
find_package(boost_filesystem REQUIRED CONFIG)
find_package(boost_nowide CONFIG REQUIRED)
find_package(boost_process REQUIRED CONFIG)
find_package(CpuFeatures CONFIG REQUIRED)

target_link_libraries(rad_system
    PUBLIC rad_common
    PUBLIC rad_container
    PUBLIC rad_io
    PUBLIC Backward::Backward
    PUBLIC Boost::asio
    PUBLIC Boost::filesystem
    PUBLIC Boost::nowide
    PUBLIC Boost::process
    PUBLIC CpuFeatures::cpu_features
)

if(RAD_BUILD_TESTS)
    add_executable(rad_system_tests)
    target_sources(rad_system_tests PRIVATE ${RAD_SYSTEM_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SYSTEM_TEST_SOURCES})
    target_link_libraries(rad_system_tests
        PRIVATE rad_system
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_system_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_system_tests)
endif()
