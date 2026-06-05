set(RAD_GUI_HEADERS
    GuiCommon.h
    GuiTimer.h
    GuiApplication.h
    GuiEventHandler.h
    Window.h
    Surface.h
    GuiTexture.h
    GuiRenderer.h
    RHI.h
    RHI/GpuDevice.h
    RHI/GpuFence.h
    RHI/GpuShader.h
    RHI/GpuBuffer.h
    RHI/GpuTexture.h
    RHI/GpuSampler.h
    RHI/GpuGraphicsPipeline.h
    RHI/GpuComputePipeline.h
    RHI/GpuTransferBuffer.h
    RHI/GpuCommandBuffer.h
    RHI/GpuPass.h
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
    RHI/GpuDevice.cpp
    RHI/GpuFence.cpp
    RHI/GpuShader.cpp
    RHI/GpuBuffer.cpp
    RHI/GpuTexture.cpp
    RHI/GpuSampler.cpp
    RHI/GpuGraphicsPipeline.cpp
    RHI/GpuComputePipeline.cpp
    RHI/GpuTransferBuffer.cpp
    RHI/GpuCommandBuffer.cpp
    RHI/GpuPass.cpp
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

if(Vulkan_FOUND)
    list(APPEND IMGUI_HEADERS imgui/backends/imgui_impl_vulkan.h)
    list(APPEND IMGUI_SOURCES imgui/backends/imgui_impl_vulkan.cpp)
else()
    message(STATUS "rad_gui: Vulkan not found; imgui Vulkan backend disabled")
endif()

set(RAD_GUI_TEST_SOURCES
    Gui.test.h
    Gui.test.cpp
    GuiApplication.test.cpp
    Window.test.h
    Window.test.cpp
    GuiRenderer.test.h
    GuiRenderer.test.cpp
)

add_library(imgui STATIC)
target_sources(imgui PRIVATE ${IMGUI_HEADERS} ${IMGUI_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/imgui FILES ${IMGUI_HEADERS} ${IMGUI_SOURCES})
set_target_properties(imgui PROPERTIES FOLDER "imported")

target_include_directories(imgui
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/imgui>
)

target_link_libraries(imgui
    PUBLIC SDL3::SDL3
)

if(Vulkan_FOUND)
    target_link_libraries(imgui PUBLIC Vulkan::Headers)
endif()

# Ensure the SDL3 headers from SDL3::SDL3 target win over the SDL3 headers bundled in VulkanSDK.
target_include_directories(imgui BEFORE
    PUBLIC "$<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>"
)

add_library(implot STATIC)
target_sources(implot PRIVATE ${IMPLOT_HEADERS} ${IMPLOT_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/implot FILES ${IMPLOT_HEADERS} ${IMPLOT_SOURCES})
set_target_properties(implot PROPERTIES FOLDER "imported")

target_include_directories(implot
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/implot>
)

target_link_libraries(implot PUBLIC imgui)

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

# Optional Vulkan-backed window/renderer (requires rad_vulkan).
if(Vulkan_FOUND AND TARGET rad_vulkan)
    list(APPEND RAD_GUI_HEADERS VulkanGuiRenderer.h VulkanWindow.h VulkanFrame.h VulkanGuiComposition.h)
    list(APPEND RAD_GUI_SOURCES VulkanGuiRenderer.cpp VulkanWindow.cpp VulkanFrame.cpp VulkanGuiComposition.cpp)
    set(RAD_GUI_SHADERS
        Shaders/Compile.py
        Shaders/GuiComposition.vert
        Shaders/GuiComposition.frag
    )
endif()

add_library(rad_gui STATIC)

target_sources(rad_gui PRIVATE ${RAD_GUI_HEADERS} ${RAD_GUI_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_GUI_HEADERS} ${RAD_GUI_SOURCES})
if(RAD_GUI_SHADERS)
    target_sources(rad_gui PRIVATE ${RAD_GUI_SHADERS})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_GUI_SHADERS})
endif()

target_include_directories(rad_gui BEFORE
    PUBLIC $<TARGET_PROPERTY:SDL3::SDL3,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(rad_gui
    PUBLIC rad_system
    PUBLIC SDL3::SDL3
    PUBLIC imgui
    PUBLIC implot
)

if(Vulkan_FOUND)
    target_link_libraries(rad_gui PUBLIC Vulkan::Headers)
endif()

if(Vulkan_FOUND AND TARGET rad_vulkan)
    target_link_libraries(rad_gui PUBLIC rad_vulkan)

    if(RAD_GUI_SHADERS)
        find_package(Vulkan REQUIRED COMPONENTS glslc)
        find_package(Python3 REQUIRED COMPONENTS Interpreter)

        set(RAD_GUI_SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
        # Match CMAKE_RUNTIME_OUTPUT_DIRECTORY (bin/ or bin/$<CONFIG>/ for multi-config generators).
        set(RAD_GUI_SHADER_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders")
        set(RAD_GUI_SHADER_COMPILE_SCRIPT ${RAD_GUI_SHADER_SOURCE_DIR}/Compile.py)

        if(NOT TARGET Vulkan::glslc AND NOT Vulkan_glslc_EXECUTABLE)
            message(FATAL_ERROR "rad_gui: glslc not found (enable Vulkan glslc component)")
        endif()

        if(TARGET Vulkan::glslc)
            set(RAD_GUI_GLSLC_ARG --glslc "$<TARGET_FILE:Vulkan::glslc>")
        else()
            set(RAD_GUI_GLSLC_ARG --glslc "${Vulkan_glslc_EXECUTABLE}")
        endif()

        add_custom_command(
            TARGET rad_gui
            PRE_BUILD
            COMMAND ${Python3_EXECUTABLE}
                ${RAD_GUI_SHADER_COMPILE_SCRIPT}
                ${RAD_GUI_GLSLC_ARG}
                --output-dir ${RAD_GUI_SHADER_OUTPUT_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Compiling GUI shaders to SPIR-V"
            VERBATIM
        )

    endif()
endif()

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
    if (WIN32)
        add_custom_command(TARGET rad_gui_tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:rad_gui_tests> $<TARGET_FILE_DIR:rad_gui_tests>
            COMMAND_EXPAND_LISTS
        )
    endif()
    # Pass a finite cap so discovered tests return under CTest (see TestEnvironment / WindowTest).
    # CTest defaults to the build directory; run from the executable dir so runtime assets resolve.
    gtest_discover_tests(rad_gui_tests
        EXTRA_ARGS --max-frames=1000
        WORKING_DIRECTORY $<TARGET_FILE_DIR:rad_gui_tests>
    )

    if(Vulkan_FOUND AND TARGET rad_vulkan)
        set(RAD_VULKAN_GUI_TEST_SOURCES
            VulkanGui.test.h
            VulkanGui.test.cpp
            VulkanWindow.test.h
            VulkanWindow.test.cpp
        )
        add_executable(rad_vulkan_gui_tests)
        target_sources(rad_vulkan_gui_tests PRIVATE ${RAD_VULKAN_GUI_TEST_SOURCES})
        source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_VULKAN_GUI_TEST_SOURCES})
        target_link_libraries(rad_vulkan_gui_tests
            PRIVATE rad_gui
            PRIVATE rad_vulkan
            PRIVATE GTest::gtest
        )
        set_target_properties(rad_vulkan_gui_tests PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
        )
        if (WIN32)
            add_custom_command(TARGET rad_vulkan_gui_tests POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_RUNTIME_DLLS:rad_vulkan_gui_tests>
                    $<TARGET_FILE_DIR:rad_vulkan_gui_tests>
                COMMAND_EXPAND_LISTS
            )
        endif()
        gtest_discover_tests(rad_vulkan_gui_tests
            EXTRA_ARGS --max-frames=1000
            WORKING_DIRECTORY $<TARGET_FILE_DIR:rad_vulkan_gui_tests>
        )
    endif()
endif()
