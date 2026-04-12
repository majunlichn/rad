#include <rad/Vulkan/VulkanCommon.h>

#include <rad/System/Application.h>
#include <rad/IO/Logging.h>

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    rad::Application app(argc, argv);

    vk::raii::Context context;

    vk::ApplicationInfo applicationInfo;
    applicationInfo.pApplicationName = "rad";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    applicationInfo.pEngineName = "rad";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    vk::InstanceCreateInfo instanceCreateInfo({}, &applicationInfo);

    vk::raii::Instance instance(context, instanceCreateInfo);

    vk::raii::PhysicalDevices physicalDevices(instance);

    for (size_t i = 0; i < physicalDevices.size(); ++i)
    {
        vk::PhysicalDeviceProperties properties = physicalDevices[i].getProperties();
        SPDLOG_INFO("Vulkan Device#{}: {}", i, properties.deviceName.data());
    }

    if (physicalDevices.empty())
    {
        SPDLOG_INFO("No Vulkan physical devices found!");
        return 0;
    }

    float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.queueFamilyIndex = 0;
    deviceQueueCreateInfo.queueCount = 1;
    deviceQueueCreateInfo.pQueuePriorities = &queuePriority;
    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

    vk::raii::PhysicalDevice physicalDevice = vk::raii::PhysicalDevices(instance).front();
    vk::raii::Device device(physicalDevice, deviceCreateInfo);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
