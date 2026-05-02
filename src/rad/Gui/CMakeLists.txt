set(RAD_GUI_HEADERS
    GuiCommon.h
    GuiApplication.h
    GuiEventHandler.h
    Window.h
    Surface.h
    GuiTexture.h
    GuiRenderer.h
    imgui/imconfig.h
    imgui/imgui_internal.h
    imgui/imgui.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/backends/imgui_impl_sdl3.h
    imgui/backends/imgui_impl_sdlrenderer3.h
)

set(RAD_GUI_SOURCES
    GuiCommon.cpp
    GuiApplication.cpp
    GuiEventHandler.cpp
    Window.cpp
    Surface.cpp
    GuiTexture.cpp
    GuiRenderer.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_sdlrenderer3.cpp
)

if (DEFINED ENV{VULKAN_SDK})
    set(VULKAN_SDK_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
    if (WIN32)
        set(VULKAN_SDK_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include")
    endif()
    list(APPEND RAD_GUI_HEADERS imgui/backends/imgui_impl_vulkan.h)
    list(APPEND RAD_GUI_SOURCES imgui/backends/imgui_impl_vulkan.cpp)
endif()

set(RAD_GUI_TEST_SOURCES
    Gui.test.h
    Gui.test.cpp
    GuiApplication.test.cpp
)

add_library(rad_gui STATIC)

target_sources(rad_gui PRIVATE ${RAD_GUI_HEADERS} ${RAD_GUI_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_GUI_HEADERS} ${RAD_GUI_SOURCES})

find_package(SDL3 CONFIG REQUIRED)

target_include_directories(rad_gui
    PUBLIC imgui
)

if (DEFINED ENV{VULKAN_SDK})
    target_include_directories(rad_gui
        PUBLIC ${VULKAN_SDK_INCLUDE_DIR}
    )
endif()

target_link_libraries(rad_gui
    PUBLIC rad_system
    PUBLIC SDL3::SDL3
)

if(RAD_BUILD_TESTS)
    add_executable(rad_gui_tests)
    target_sources(rad_gui_tests PRIVATE ${RAD_GUI_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_GUI_TEST_SOURCES})
    target_link_libraries(rad_gui_tests
        PRIVATE rad_gui
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_gui_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_gui_tests)
endif()
