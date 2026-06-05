set(RAD_VULKAN_HEADERS
    VulkanCommon.h
    VulkanCore.h
    VulkanShader.h
    Core/VulkanBuffer.h
    Core/VulkanCommand.h
    Core/VulkanDescriptor.h
    Core/VulkanDevice.h
    Core/VulkanEvent.h
    Core/VulkanFence.h
    Core/VulkanFramebuffer.h
    Core/VulkanImage.h
    Core/VulkanInstance.h
    Core/VulkanPipeline.h
    Core/VulkanQuery.h
    Core/VulkanRenderPass.h
    Core/VulkanSampler.h
    Core/VulkanSemaphore.h
    Core/VulkanSurface.h
    Core/VulkanSwapchain.h
    Compiler/internal/shaderc_util.h
    Compiler/GLSLCompiler.h
    Compiler/SpvContext.h
)

set(RAD_VULKAN_SOURCES
    VulkanCommon.cpp
    VulkanShader.cpp
    Core/VulkanBuffer.cpp
    Core/VulkanCommand.cpp
    Core/VulkanDescriptor.cpp
    Core/VulkanDevice.cpp
    Core/VulkanEvent.cpp
    Core/VulkanFence.cpp
    Core/VulkanFramebuffer.cpp
    Core/VulkanImage.cpp
    Core/VulkanInstance.cpp
    Core/VulkanPipeline.cpp
    Core/VulkanQuery.cpp
    Core/VulkanRenderPass.cpp
    Core/VulkanSampler.cpp
    Core/VulkanSemaphore.cpp
    Core/VulkanSurface.cpp
    Core/VulkanSwapchain.cpp
    Compiler/internal/shaderc_util.cpp
    Compiler/GLSLCompiler.cpp
    Compiler/SpvContext.cpp
)

set(RAD_VULKAN_TEST_SOURCES
    Vulkan.test.h
    Vulkan.test.cpp
    VulkanCommon.test.cpp
)

add_library(rad_vulkan STATIC)

target_sources(rad_vulkan PRIVATE ${RAD_VULKAN_HEADERS} ${RAD_VULKAN_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_VULKAN_HEADERS} ${RAD_VULKAN_SOURCES})

# https://cmake.org/cmake/help/latest/module/FindVulkan.html
find_package(Vulkan REQUIRED COMPONENTS SPIRV-Tools glslc)

set(VULKAN_SDK_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
set(VULKAN_SDK_LIB_DIR "$ENV{VULKAN_SDK}/lib")
if (WIN32)
    set(VULKAN_SDK_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include")
    set(VULKAN_SDK_LIB_DIR "$ENV{VULKAN_SDK}/Lib")
endif()

target_link_libraries(rad_vulkan
    PUBLIC rad_common
    PUBLIC rad_container
    PUBLIC rad_io
    PUBLIC Vulkan::Headers
)

if(WIN32)
    target_link_libraries(rad_vulkan
        PUBLIC "${VULKAN_SDK_LIB_DIR}/shaderc_shared.lib"
        PUBLIC "${VULKAN_SDK_LIB_DIR}/SPIRV-Tools-shared.lib"
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(rad_vulkan
        PUBLIC "${VULKAN_SDK_LIB_DIR}/libshaderc_shared.so"
        PUBLIC "${VULKAN_SDK_LIB_DIR}/libSPIRV-Tools-shared.so"
    )
endif()

if(RAD_BUILD_TESTS)
    add_executable(rad_vulkan_tests)
    target_sources(rad_vulkan_tests PRIVATE ${RAD_VULKAN_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_VULKAN_TEST_SOURCES})
    target_link_libraries(rad_vulkan_tests
        PRIVATE rad_system
        PRIVATE rad_vulkan
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_vulkan_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_vulkan_tests)
endif()
