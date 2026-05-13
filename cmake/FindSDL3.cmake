# SDL3 / SDL3_mixer: explicit *_DIR, then find_package, then FetchContent.
#
# This file is meant to be included explicitly: include(.../FindSDL3.cmake)
# It is not a CMake MODULE-mode Find script for find_package(SDL3).
#
# Hints: cache or environment to the directory that contains the package Config file, e.g.
#   SDL3_DIR=.../lib/cmake/SDL3
#   SDL3_mixer_DIR=.../lib/cmake/SDL3_mixer
#
# Functions are idempotent: safe to call more than once if the imported target already exists.
#
# Fetch tags (only when find_package cannot locate the package):
#   -DSDL3_GIT_TAG=release-3.4.8
#   -DSDL3_MIXER_GIT_TAG=release-3.2.2
# FetchContent uses GIT_SHALLOW for both repos (--depth 1) to reduce clone time and disk use.

include_guard(GLOBAL)

include(FetchContent)

set(SDL3_GIT_TAG "release-3.4.8" CACHE STRING "Git tag for FetchContent SDL3 when find_package fails")
set(SDL3_MIXER_GIT_TAG "release-3.2.2" CACHE STRING "Git tag for FetchContent SDL3_mixer when find_package fails")

function(sdl3_resolve)
    if(TARGET SDL3::SDL3)
        return()
    endif()

    set(_sdl3_config_dir "${SDL3_DIR}")
    if(NOT _sdl3_config_dir AND DEFINED ENV{SDL3_DIR} AND NOT "$ENV{SDL3_DIR}" STREQUAL "")
        set(_sdl3_config_dir "$ENV{SDL3_DIR}")
    endif()

    if(_sdl3_config_dir)
        cmake_path(NORMAL_PATH _sdl3_config_dir "${_sdl3_config_dir}")
        set(SDL3_DIR "${_sdl3_config_dir}" CACHE PATH "Directory containing SDL3Config.cmake" FORCE)
        find_package(SDL3 CONFIG REQUIRED)
        return()
    endif()

    find_package(SDL3 CONFIG QUIET)
    if(SDL3_FOUND)
        message(STATUS "SDL3_DIR=${SDL3_DIR}")
        return()
    endif()

    message(STATUS "SDL3: not found in search path; FetchContent libsdl-org/SDL @ ${SDL3_GIT_TAG}")

    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG "${SDL3_GIT_TAG}"
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE)

    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_TESTS OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(SDL3)

    if(sdl3_POPULATED)
        message(STATUS "SDL3: FetchContent source: ${sdl3_SOURCE_DIR}")
        message(STATUS "SDL3: FetchContent binary: ${sdl3_BINARY_DIR}")
        if(NOT TARGET SDL3::SDL3)
            message(FATAL_ERROR "SDL3: FetchContent completed but imported target SDL3::SDL3 is missing")
        endif()
    else()
        message(FATAL_ERROR "SDL3: FetchContent failed!")
    endif()
endfunction()

function(sdl3_mixer_resolve)
    if(TARGET SDL3_mixer::SDL3_mixer)
        return()
    endif()

    if(NOT TARGET SDL3::SDL3)
        message(FATAL_ERROR "SDL3_mixer: SDL3::SDL3 is not defined, call sdl3_resolve() first.")
    endif()

    set(_sdl3_mixer_config_dir "${SDL3_mixer_DIR}")
    if(NOT _sdl3_mixer_config_dir AND DEFINED ENV{SDL3_mixer_DIR} AND
       NOT "$ENV{SDL3_mixer_DIR}" STREQUAL "")
        set(_sdl3_mixer_config_dir "$ENV{SDL3_mixer_DIR}")
    endif()

    if(_sdl3_mixer_config_dir)
        cmake_path(NORMAL_PATH _sdl3_mixer_config_dir "${_sdl3_mixer_config_dir}")
        set(SDL3_mixer_DIR "${_sdl3_mixer_config_dir}" CACHE PATH
            "Directory containing SDL3_mixerConfig.cmake" FORCE)
        find_package(SDL3_mixer CONFIG REQUIRED)
        return()
    endif()

    find_package(SDL3_mixer CONFIG QUIET)
    if(SDL3_mixer_FOUND)
        message(STATUS "SDL3_mixer_DIR=${SDL3_mixer_DIR}")
        return()
    endif()

    message(
        STATUS
        "SDL3_mixer: not found in search path; FetchContent libsdl-org/SDL_mixer @ ${SDL3_MIXER_GIT_TAG} (submodules: recursive)"
    )

    FetchContent_Declare(
        SDL3_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG "${SDL3_MIXER_GIT_TAG}"
        GIT_SHALLOW TRUE
        GIT_SUBMODULES_RECURSE TRUE
        GIT_PROGRESS TRUE)

    FetchContent_MakeAvailable(SDL3_mixer)

    if(sdl3_mixer_POPULATED)
        message(STATUS "SDL3_mixer: FetchContent source: ${sdl3_mixer_SOURCE_DIR}")
        message(STATUS "SDL3_mixer: FetchContent binary: ${sdl3_mixer_BINARY_DIR}")
        if(NOT TARGET SDL3_mixer::SDL3_mixer)
            message(FATAL_ERROR "SDL3_mixer: FetchContent completed but imported target SDL3_mixer::SDL3_mixer is missing")
        endif()
    else()
        message(
            FATAL_ERROR
            "SDL3_mixer: FetchContent failed!")
    endif()
endfunction()
