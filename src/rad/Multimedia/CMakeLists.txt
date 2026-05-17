set(RAD_MULTIMEDIA_HEADERS
    MultimediaCommon.h
    AudioDevice.h
    AudioStream.h
    AudioMixer.h
    Camera.h
)

set(RAD_MULTIMEDIA_SOURCES
    MultimediaCommon.cpp
    AudioDevice.cpp
    AudioStream.cpp
    AudioMixer.cpp
    Camera.cpp
)

set(RAD_MULTIMEDIA_TEST_SOURCES
    Multimedia.test.h
    Multimedia.test.cpp
    AudioDevice.test.cpp
    Camera.test.cpp
)

# SDL3_mixer is resolved in top-level CMakeLists (cmake/FindSDL3.cmake) before this directory is added.

add_library(rad_multimedia STATIC)

target_sources(rad_multimedia PRIVATE ${RAD_MULTIMEDIA_HEADERS} ${RAD_MULTIMEDIA_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_MULTIMEDIA_HEADERS} ${RAD_MULTIMEDIA_SOURCES})

target_link_libraries(rad_multimedia
    PUBLIC rad_common
    PUBLIC rad_container
    PUBLIC rad_io
    PUBLIC rad_system
    PUBLIC SDL3::SDL3
    PUBLIC SDL3_mixer::SDL3_mixer
)

if(RAD_BUILD_TESTS)
    add_executable(rad_multimedia_tests)
    target_sources(rad_multimedia_tests PRIVATE ${RAD_MULTIMEDIA_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_MULTIMEDIA_TEST_SOURCES})
    target_link_libraries(rad_multimedia_tests
        PRIVATE rad_multimedia
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_multimedia_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_multimedia_tests)
endif()
