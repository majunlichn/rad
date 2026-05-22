set(RAD_SCENE_HEADERS
    Entity.h
    Scene.h
)

set(RAD_SCENE_SOURCES
    Entity.cpp
    Scene.cpp
)

add_library(rad_scene STATIC)

target_sources(rad_scene PRIVATE ${RAD_SCENE_HEADERS} ${RAD_SCENE_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SCENE_HEADERS} ${RAD_SCENE_SOURCES})

find_package(EnTT CONFIG REQUIRED)

target_link_libraries(rad_scene
    PUBLIC rad_common
    PUBLIC rad_container
    PUBLIC EnTT::EnTT
)

set(RAD_SCENE_TEST_SOURCES
    Scene.test.h
    Scene.test.cpp
)

if(RAD_BUILD_TESTS)
    add_executable(rad_scene_tests)
    target_sources(rad_scene_tests PRIVATE ${RAD_SCENE_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SCENE_TEST_SOURCES})
    target_link_libraries(rad_scene_tests
        PRIVATE rad_scene
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_scene_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_scene_tests)
endif()
