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

set(SDL3_MIXER_INSTALL_DIR "" CACHE PATH
    "Optional SDL3_mixer install prefix (folder that contains lib/cmake/SDL3_mixer); empty uses default search")

if(SDL3_MIXER_INSTALL_DIR)
    find_package(SDL3_mixer CONFIG REQUIRED
        PATHS "${SDL3_INSTALL_DIR}"
        PATH_SUFFIXES lib/cmake/SDL3_mixer cmake
        NO_DEFAULT_PATH)
else()
    find_package(SDL3_mixer CONFIG REQUIRED)
endif()

message(STATUS "SDL3_mixer_DIR=${SDL3_mixer_DIR}")

add_library(rad_multimedia STATIC)

target_sources(rad_multimedia PRIVATE ${RAD_MULTIMEDIA_HEADERS} ${RAD_MULTIMEDIA_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_MULTIMEDIA_HEADERS} ${RAD_MULTIMEDIA_SOURCES})

target_link_libraries(rad_multimedia
    PUBLIC rad_gui
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
