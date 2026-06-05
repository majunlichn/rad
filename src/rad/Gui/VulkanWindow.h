#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/Window.h>

#include <rad/Vulkan/VulkanCore.h>

namespace rad
{

// SDL window with Vulkan surface. Optional VulkanFrame owns swapchain and presentation.
// VulkanInstance and VulkanDevice must outlive this window.
class VulkanWindow : public Window
{
public:
    struct CreateInfo
    {
        Ref<VulkanInstance> instance;
        Ref<VulkanDevice> device;
        cstring_view title;
        int width = 1280;
        int height = 720;
        SDL_WindowFlags extraWindowFlags = 0;
    };

    VulkanWindow();
    ~VulkanWindow() override;

    bool Create(const CreateInfo& info);
    void Destroy() override;

    VulkanInstance* GetVulkanInstance() const { return m_instance.get(); }
    VulkanDevice* GetVulkanDevice() const { return m_device.get(); }
    VulkanSurface* GetSurface() const { return m_surface.get(); }

private:
    bool CreateSurface();
    void DestroyVulkanResources();

    Ref<VulkanInstance> m_instance;
    Ref<VulkanDevice> m_device;
    Ref<VulkanSurface> m_surface;

}; // class VulkanWindow

} // namespace rad
