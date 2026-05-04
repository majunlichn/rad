set(RAD_GUI_HEADERS
    GuiCommon.h
    GuiTimer.h
    GuiApplication.h
    GuiEventHandler.h
    Window.h
    Surface.h
    GuiTexture.h
    GuiRenderer.h
)

set(RAD_GUI_SOURCES
    GuiCommon.cpp
    GuiTimer.cpp
    GuiApplication.cpp
    GuiEventHandler.cpp
    Window.cpp
    Surface.cpp
    GuiTexture.cpp
    GuiRenderer.cpp
)

set(IMGUI_HEADERS
    imgui/imconfig.h
    imgui/imgui_internal.h
    imgui/imgui.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/backends/imgui_impl_sdl3.h
    imgui/backends/imgui_impl_sdlrenderer3.h
)

set(IMGUI_SOURCES
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui.cpp
    imgui/backends/imgui_impl_sdl3.cpp
    imgui/backends/imgui_impl_sdlrenderer3.cpp
)

set(IMPLOT_HEADERS
    implot/implot_internal.h
    implot/implot.h
)

set(IMPLOT_SOURCES
    implot/implot_demo.cpp
    implot/implot_items.cpp
    implot/implot.cpp
)

if (DEFINED ENV{VULKAN_SDK})
    list(APPEND IMGUI_HEADERS imgui/backends/imgui_impl_vulkan.h)
    list(APPEND IMGUI_SOURCES imgui/backends/imgui_impl_vulkan.cpp)
endif()

set(RAD_GUI_TEST_SOURCES
    Gui.test.h
    Gui.test.cpp
    GuiApplication.test.cpp
    Window.test.h
    Window.test.cpp
)

find_package(SDL3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_library(imgui STATIC)
target_sources(imgui PRIVATE ${IMGUI_HEADERS} ${IMGUI_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/imgui FILES ${IMGUI_HEADERS} ${IMGUI_SOURCES})
set_target_properties(imgui PROPERTIES FOLDER "imported")

target_include_directories(imgui
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
)

# Ensure the SDL3 headers from vcpkg win over SDL3 headers bundled in VulkanSDK.
target_include_directories(imgui BEFORE
    PUBLIC $<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(imgui
    PRIVATE SDL3::SDL3
    PRIVATE Vulkan::Headers
)

add_library(implot STATIC)
target_sources(implot PRIVATE ${IMPLOT_HEADERS} ${IMPLOT_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/implot FILES ${IMPLOT_HEADERS} ${IMPLOT_SOURCES})
set_target_properties(implot PROPERTIES FOLDER "imported")

target_include_directories(implot
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/implot>
)

target_link_libraries(implot
    PUBLIC imgui
)

if(MSVC)
    target_compile_options(imgui PRIVATE /W0 /WX-)
    target_compile_options(implot PRIVATE /W0 /WX-)
else()
    target_compile_options(imgui PRIVATE -w)
    target_compile_options(implot PRIVATE -w)
endif()

add_library(rad_gui STATIC)

target_sources(rad_gui PRIVATE ${RAD_GUI_HEADERS} ${RAD_GUI_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_GUI_HEADERS} ${RAD_GUI_SOURCES})

target_include_directories(rad_gui BEFORE
    PUBLIC $<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(rad_gui
    PUBLIC rad_system
    PUBLIC SDL3::SDL3
    PUBLIC Vulkan::Headers
    PUBLIC imgui
    PUBLIC implot
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
    # Pass a finite cap so discovered tests return under CTest (see TestEnvironment / MainWindow).
    gtest_discover_tests(rad_gui_tests EXTRA_ARGS --max-frames=1000)
endif()
