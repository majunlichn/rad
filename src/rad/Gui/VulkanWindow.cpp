#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/VulkanWindow.h>

#include <SDL3/SDL_vulkan.h>

namespace rad
{

VulkanWindow::VulkanWindow() = default;

VulkanWindow::~VulkanWindow()
{
    Destroy();
}

bool VulkanWindow::Create(const CreateInfo& info)
{
    if (!info.instance || !info.device)
    {
        RAD_LOG_GUI(err, "VulkanWindow::Create requires VulkanInstance and VulkanDevice");
        return false;
    }

    m_instance = info.instance;
    m_device = info.device;

    SDL_WindowFlags flags = info.extraWindowFlags | SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;
    if (!Window::Create(info.title, info.width, info.height, flags))
    {
        return false;
    }

    if (!CreateSurface())
    {
        Destroy();
        return false;
    }

    RAD_LOG_GUI(info, "VulkanWindow created: {}x{}", info.width, info.height);
    return true;
}

void VulkanWindow::Destroy()
{
    DestroyVulkanResources();
    Window::Destroy();
}

bool VulkanWindow::CreateSurface()
{
    VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
    if (!SDL_Vulkan_CreateSurface(GetHandle(), static_cast<VkInstance>(m_instance->GetHandle()),
                                  nullptr, &surfaceHandle))
    {
        RAD_LOG_GUI(err, "SDL_Vulkan_CreateSurface failed: {}", SDL_GetError());
        return false;
    }

    m_surface = m_instance->CreateSurface(vk::SurfaceKHR(surfaceHandle));
    if (!m_surface)
    {
        RAD_LOG_GUI(err, "VulkanWindow failed to create VulkanSurface");
        return false;
    }
    return true;
}

void VulkanWindow::DestroyVulkanResources()
{
    m_surface.reset();
}

} // namespace rad
