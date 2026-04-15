set(RAD_VULKAN_HEADERS
    VulkanCommon.h
    VulkanInstance.h
    VulkanDevice.h
    VulkanCommand.h
)

set(RAD_VULKAN_SOURCES
    VulkanCommon.cpp
    VulkanInstance.cpp
    VulkanDevice.cpp
    VulkanCommand.cpp
)

set(RAD_VULKAN_TEST_SOURCES
    VulkanCommon.test.cpp
    VulkanTestContext.h
    VulkanTestContext.cpp
    VulkanTestMain.cpp
)

add_library(rad_vulkan STATIC)
target_sources(rad_vulkan PRIVATE ${RAD_VULKAN_HEADERS} ${RAD_VULKAN_SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}
    FILES ${RAD_VULKAN_HEADERS} ${RAD_VULKAN_SOURCES}
)

find_package(VulkanHeaders CONFIG)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

target_link_libraries(rad_vulkan
    PUBLIC rad
    PUBLIC Vulkan::Headers
    PUBLIC GPUOpen::VulkanMemoryAllocator
)

if(RAD_BUILD_TESTS)
    enable_testing()

    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)

    add_executable(rad_vulkan_tests)
    target_sources(rad_vulkan_tests PRIVATE ${RAD_VULKAN_TEST_SOURCES})
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_VULKAN_TEST_SOURCES})
    target_include_directories(rad_vulkan_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(rad_vulkan_tests
        PRIVATE rad_vulkan
        PRIVATE GTest::gtest
    )
    set_target_properties(rad_vulkan_tests PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    gtest_discover_tests(rad_vulkan_tests)
endif()
