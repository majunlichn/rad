set(RAD_IO_HEADERS
    File.h
    Image.h
    Logging.h
)

set(RAD_IO_SOURCES
    File.cpp
    Image.cpp
    Logging.cpp
)

set(RAD_IO_TEST_SOURCES
    IO.test.h
    IO.test.cpp
    Image.test.cpp
)

add_library(rad_io STATIC)

target_sources(rad_io PRIVATE ${RAD_IO_HEADERS} ${RAD_IO_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_IO_HEADERS} ${RAD_IO_SOURCES})

find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)

target_link_libraries(rad_io
    PUBLIC rad_common
    PUBLIC rad_container
    PUBLIC spdlog::spdlog
)

if(RAD_BUILD_TESTS)
    add_executable(rad_io_tests)
    target_sources(rad_io_tests PRIVATE ${RAD_IO_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_IO_TEST_SOURCES})
    target_link_libraries(rad_io_tests
        PRIVATE rad_io
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_io_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_io_tests)
endif()
