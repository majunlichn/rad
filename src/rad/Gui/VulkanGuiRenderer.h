#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/VulkanWindow.h>

#include <rad/Vulkan/VulkanCore.h>

#include <SDL3/SDL.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_vulkan.h"

namespace rad
{

class VulkanFrame;
class VulkanWindow;

// Vulkan-backed ImGui renderer. Draws ImGui into VulkanFrame's per-slot GUI color targets (UNORM).
// ImGui has no linear-framebuffer mode; vertex colors are display-referred sRGB. The composition pass
// decodes GUI samples to linear before blending with the linear scene target.
// Construction initializes ImGui SDL3/Vulkan backends; throws std::runtime_error on failure.
// The associated VulkanWindow and VulkanFrame must outlive the renderer.
class VulkanGuiRenderer : public RefCounted<VulkanGuiRenderer>
{
public:
    // Validates window/frame, then the ImGui backends.
    VulkanGuiRenderer(VulkanWindow* window, VulkanFrame* frame);
    // Calls Destroy().
    ~VulkanGuiRenderer();

    VulkanWindow* GetWindow() const { return m_window; }
    VulkanFrame* GetFrame() const { return m_frame; }
    // https://wiki.libsdl.org/SDL3/SDL_GetWindowFromID
    SDL_Window* GetRenderWindowHandle() const;
    // Delegates to Window::GetSizeInPixels.
    bool GetOutputSize(int* w, int* h);

    // GUI color target for the frame currently in flight (owned by VulkanFrame). Null when there is
    // no active frame.
    VulkanImage* GetGuiColorImage() const;
    VulkanImageView* GetGuiColorImageView() const;

    // ImGui_ImplVulkan_SetMinImageCount after swapchain recreation.
    void SetMinImageCount(uint32_t minImageCount);

    // ImGui_ImplSDL3_NewFrame / ImGui_ImplVulkan_NewFrame / ImGui::NewFrame. Called from
    // VulkanFrame::BeginFrame().
    void BeginFrame();
    // ImGui::Render into the active GUI color image (dynamic rendering), then transitions it to
    // eShaderReadOnlyOptimal for the composite pass. Called from VulkanFrame::EndFrame().
    void Render();

    // ImGui_ImplSDL3_ProcessEvent.
    bool ProcessEvent(const SDL_Event& event);

private:
    bool Init();
    // ImGui context/IO/style + SDL3 backend setup, then the Vulkan backend.
    bool InitImGui();
    // Builds the ImGui Vulkan backend for the fixed UI-target format.
    bool InitImGuiVulkanBackend();
    // Shuts down ImGui backends.
    void Destroy();

    VulkanDevice* GetDevice() const;

    VulkanWindow* m_window = nullptr;
    VulkanFrame* m_frame = nullptr;

    ImGuiContext* m_imgui = nullptr;

}; // class VulkanGuiRenderer

} // namespace rad
