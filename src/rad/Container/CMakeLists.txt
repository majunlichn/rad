set(RAD_CONTAINER_HEADERS
    SmallVector.h
    Span.h
)

set(RAD_CONTAINER_SOURCES
    Container.cpp
)

set(RAD_CONTAINER_TEST_SOURCES
    Container.test.h
    Container.test.cpp
    Span.test.cpp
)

add_library(rad_container STATIC)

target_sources(rad_container PRIVATE ${RAD_CONTAINER_HEADERS} ${RAD_CONTAINER_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_CONTAINER_HEADERS} ${RAD_CONTAINER_SOURCES})

target_link_libraries(rad_container
    PUBLIC rad_common
)

if(RAD_BUILD_TESTS)
    add_executable(rad_container_tests)
    target_sources(rad_container_tests PRIVATE ${RAD_CONTAINER_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_CONTAINER_TEST_SOURCES})
    target_link_libraries(rad_container_tests
        PRIVATE rad_container
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_container_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_container_tests)
endif()
