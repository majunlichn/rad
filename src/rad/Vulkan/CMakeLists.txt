set(RAD_VULKAN_HEADERS
    internal/shaderc_util.h
    GLSLCompiler.h
    SpvContext.h
    VulkanBuffer.h
    VulkanCommand.h
    VulkanCommon.h
    VulkanDescriptor.h
    VulkanDevice.h
    VulkanEvent.h
    VulkanFence.h
    VulkanFramebuffer.h
    VulkanImage.h
    VulkanInstance.h
    VulkanPipeline.h
    VulkanQuery.h
    VulkanRenderPass.h
    VulkanSampler.h
    VulkanSemaphore.h
    VulkanSurface.h
    VulkanSwapchain.h
)

set(RAD_VULKAN_SOURCES
    internal/shaderc_util.cpp
    GLSLCompiler.cpp
    SpvContext.cpp
    VulkanBuffer.cpp
    VulkanCommand.cpp
    VulkanCommon.cpp
    VulkanDescriptor.cpp
    VulkanDevice.cpp
    VulkanEvent.cpp
    VulkanFence.cpp
    VulkanFramebuffer.cpp
    VulkanImage.cpp
    VulkanInstance.cpp
    VulkanPipeline.cpp
    VulkanQuery.cpp
    VulkanRenderPass.cpp
    VulkanSampler.cpp
    VulkanSemaphore.cpp
    VulkanSurface.cpp
    VulkanSwapchain.cpp
)

set(RAD_VULKAN_TEST_SOURCES
    Vulkan.test.h
    Vulkan.test.cpp
    VulkanCommon.test.cpp
)

add_library(rad_vulkan STATIC)

target_sources(rad_vulkan PRIVATE ${RAD_VULKAN_HEADERS} ${RAD_VULKAN_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_VULKAN_HEADERS} ${RAD_VULKAN_SOURCES})

set(VULKAN_SDK_INCLUDE_DIR "$ENV{VULKAN_SDK}/include")
set(VULKAN_SDK_LIB_DIR "$ENV{VULKAN_SDK}/lib")
if (WIN32)
    set(VULKAN_SDK_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include")
    set(VULKAN_SDK_LIB_DIR "$ENV{VULKAN_SDK}/Lib")
endif()

find_package(boost_outcome CONFIG REQUIRED)

target_include_directories(rad_vulkan
    PUBLIC ${VULKAN_SDK_INCLUDE_DIR}
)

target_link_libraries(rad_vulkan
    PUBLIC rad_common
    PUBLIC rad_container
    PUBLIC rad_io
    PUBLIC Boost::outcome
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
