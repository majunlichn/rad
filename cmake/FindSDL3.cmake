# SDL3 / SDL3_mixer — include this file explicitly (not a CMake Find-module).
#
# Parent may set RAD_SDL3_INSTALL_DIR / RAD_SDL3_MIXER_INSTALL_DIR when
# external/<lib>/installed exists. Resolution order per library:
#   1. SDL3_DIR / SDL3_mixer_DIR (cache or environment)
#   2. RAD_SDL3_INSTALL_DIR / RAD_SDL3_MIXER_INSTALL_DIR
#   3. find_package(... CONFIG) on the system search path

include_guard()

function(_rad_pick_sdl_config_dir OUT_VAR CONFIG_FILE)
    foreach(_dir IN LISTS ARGN)
        if(_dir STREQUAL "")
            continue()
        endif()
        if(EXISTS "${_dir}/${CONFIG_FILE}")
            cmake_path(SET _dir NORMALIZE "${_dir}")
            set("${OUT_VAR}" "${_dir}" PARENT_SCOPE)
            return()
        endif()
    endforeach()
    set("${OUT_VAR}" "" PARENT_SCOPE)
endfunction()

function(sdl3_resolve)
    if(TARGET SDL3::SDL3)
        return()
    endif()

    set(_candidates "")
    if(DEFINED SDL3_DIR AND NOT "${SDL3_DIR}" STREQUAL "")
        list(APPEND _candidates "${SDL3_DIR}")
    elseif(DEFINED ENV{SDL3_DIR} AND NOT "$ENV{SDL3_DIR}" STREQUAL "")
        list(APPEND _candidates "$ENV{SDL3_DIR}")
    endif()
    if(DEFINED RAD_SDL3_INSTALL_DIR AND IS_DIRECTORY "${RAD_SDL3_INSTALL_DIR}")
        if(EXISTS "${RAD_SDL3_INSTALL_DIR}/cmake/SDL3Config.cmake")
            list(APPEND _candidates "${RAD_SDL3_INSTALL_DIR}/cmake")
        endif()
        if(EXISTS "${RAD_SDL3_INSTALL_DIR}/lib/cmake/SDL3/SDL3Config.cmake")
            list(APPEND _candidates "${RAD_SDL3_INSTALL_DIR}/lib/cmake/SDL3")
        endif()
    endif()

    _rad_pick_sdl_config_dir(_config_dir SDL3Config.cmake ${_candidates})
    if(_config_dir)
        set(SDL3_DIR "${_config_dir}" CACHE PATH "Directory containing SDL3Config.cmake" FORCE)
    endif()

    find_package(SDL3 CONFIG QUIET)
    if(NOT TARGET SDL3::SDL3)
        message(FATAL_ERROR
            "SDL3 not found. Run python setup.py, or install SDL3 system-wide.")
    endif()
    message(STATUS "SDL3 config directory: ${SDL3_DIR}")
endfunction()

function(sdl3_mixer_resolve)
    if(TARGET SDL3_mixer::SDL3_mixer)
        return()
    endif()

    if(NOT TARGET SDL3::SDL3)
        message(FATAL_ERROR "SDL3_mixer requires SDL3; call sdl3_resolve() first.")
    endif()

    set(_candidates "")
    if(DEFINED SDL3_mixer_DIR AND NOT "${SDL3_mixer_DIR}" STREQUAL "")
        list(APPEND _candidates "${SDL3_mixer_DIR}")
    elseif(DEFINED ENV{SDL3_mixer_DIR} AND NOT "$ENV{SDL3_mixer_DIR}" STREQUAL "")
        list(APPEND _candidates "$ENV{SDL3_mixer_DIR}")
    endif()
    if(DEFINED RAD_SDL3_MIXER_INSTALL_DIR AND IS_DIRECTORY "${RAD_SDL3_MIXER_INSTALL_DIR}")
        if(EXISTS "${RAD_SDL3_MIXER_INSTALL_DIR}/cmake/SDL3_mixerConfig.cmake")
            list(APPEND _candidates "${RAD_SDL3_MIXER_INSTALL_DIR}/cmake")
        endif()
        if(EXISTS "${RAD_SDL3_MIXER_INSTALL_DIR}/lib/cmake/SDL3_mixer/SDL3_mixerConfig.cmake")
            list(APPEND _candidates "${RAD_SDL3_MIXER_INSTALL_DIR}/lib/cmake/SDL3_mixer")
        endif()
    endif()

    _rad_pick_sdl_config_dir(_config_dir SDL3_mixerConfig.cmake ${_candidates})
    if(_config_dir)
        set(SDL3_mixer_DIR "${_config_dir}" CACHE PATH
            "Directory containing SDL3_mixerConfig.cmake" FORCE)
    endif()

    find_package(SDL3_mixer CONFIG QUIET)
    if(NOT TARGET SDL3_mixer::SDL3_mixer)
        message(FATAL_ERROR
            "SDL3_mixer not found. Run python setup.py, or install SDL3_mixer system-wide.")
    endif()
    message(STATUS "SDL3_mixer config directory: ${SDL3_mixer_DIR}")
endfunction()
