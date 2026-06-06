#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/VulkanGuiRenderer.h>

#include <rad/Gui/VulkanFrame.h>
#include <rad/Gui/VulkanWindow.h>
#include <rad/System/StackTrace.h>

#include <SDL3/SDL_vulkan.h>

#include "backends/imgui_impl_vulkan.h"

#include <array>
#include <stdexcept>

namespace rad
{

static void CheckVkResult(VkResult result)
{
    if (result == VK_SUCCESS) [[likely]]
    {
        return;
    }

    RAD_LOG_GUI(err, "ImGui Vulkan: {} ({})", vk::to_string(static_cast<vk::Result>(result)),
                static_cast<int>(result));

    const std::string stackTrace = PrintStackTrace();
    if (!stackTrace.empty())
    {
        RAD_LOG_GUI(err, "Stack trace:\n{}", stackTrace);
    }
}

VulkanGuiRenderer::VulkanGuiRenderer(VulkanWindow* window, VulkanFrame* frame) :
    m_window(window),
    m_frame(frame)
{
    if (!Init())
    {
        Destroy();
        throw std::runtime_error("Failed to create VulkanGuiRenderer");
    }
}

VulkanGuiRenderer::~VulkanGuiRenderer()
{
    Destroy();
}

VulkanDevice* VulkanGuiRenderer::GetDevice() const
{
    return m_window ? m_window->GetVulkanDevice() : nullptr;
}

bool VulkanGuiRenderer::Init()
{
    if (!m_window || !m_window->GetHandle())
    {
        RAD_LOG_GUI(err, "VulkanGuiRenderer requires a created Window");
        return false;
    }
    if (!m_frame || !m_frame->GetSwapchain())
    {
        RAD_LOG_GUI(err, "VulkanGuiRenderer requires an initialized VulkanFrame");
        return false;
    }
    if (!m_window->GetVulkanInstance() || !GetDevice())
    {
        RAD_LOG_GUI(err, "VulkanGuiRenderer is missing required Vulkan handles");
        return false;
    }
    const uint32_t minImageCount = m_frame->GetSwapchainMinImageCount();
    if (m_frame->GetSwapchain()->GetImageCount() < minImageCount || minImageCount < 2)
    {
        RAD_LOG_GUI(err, "VulkanGuiRenderer has invalid imageCount/minImageCount");
        return false;
    }
    if (!m_frame->GetGuiColorImage(0))
    {
        RAD_LOG_GUI(err, "VulkanGuiRenderer requires GUI color targets on VulkanFrame");
        return false;
    }

    return InitImGui();
}

VulkanImage* VulkanGuiRenderer::GetGuiColorImage() const
{
    return m_frame ? m_frame->GetGuiColorImage() : nullptr;
}

VulkanImageView* VulkanGuiRenderer::GetGuiColorImageView() const
{
    return m_frame ? m_frame->GetGuiColorImageView() : nullptr;
}

bool VulkanGuiRenderer::InitImGui()
{
    assert(m_window);
    assert(m_frame);

    IMGUI_CHECKVERSION();
    m_imgui = ImGui::CreateContext();
    if (!m_imgui)
    {
        RAD_LOG_GUI(err, "ImGui::CreateContext failed!");
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    float windowScale = m_window->GetDisplayScale();
    if (windowScale > 0)
    {
        style.ScaleAllSizes(windowScale);
        style.FontScaleDpi = windowScale * 1.2f;
    }

    if (!ImGui_ImplSDL3_InitForVulkan(m_window->GetHandle()))
    {
        RAD_LOG_GUI(err, "ImGui_ImplSDL3_InitForVulkan failed!");
        return false;
    }

    return InitImGuiVulkanBackend();
}

bool VulkanGuiRenderer::InitImGuiVulkanBackend()
{
    assert(m_window);
    assert(m_frame);

    VulkanInstance* instance = m_window->GetVulkanInstance();
    VulkanDevice* device = GetDevice();
    const uint32_t queueFamily = device->GetQueueFamilyIndex(VulkanQueueFamily::Universal);

    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.ApiVersion = instance->GetApiVersion();
    initInfo.Instance = static_cast<VkInstance>(instance->GetHandle());
    initInfo.PhysicalDevice = static_cast<VkPhysicalDevice>(device->GetPhysicalDevice());
    initInfo.Device = static_cast<VkDevice>(device->GetHandle());
    initInfo.QueueFamily = queueFamily;
    initInfo.Queue = static_cast<VkQueue>(device->GetQueue(VulkanQueueFamily::Universal));
    initInfo.DescriptorPool = VK_NULL_HANDLE;
    initInfo.DescriptorPoolSize = 1024;
    initInfo.MinImageCount = m_frame->GetSwapchainMinImageCount();
    initInfo.ImageCount = m_frame->GetSwapchain()->GetImageCount();
    initInfo.PipelineCache = VK_NULL_HANDLE;
    initInfo.Allocator = nullptr;
    initInfo.CheckVkResultFn = CheckVkResult;
    initInfo.MinAllocationSize = 1024 * 1024;

    initInfo.UseDynamicRendering = true;
    initInfo.PipelineInfoMain.RenderPass = VK_NULL_HANDLE;
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    const VkFormat colorAttachmentFormat =
        static_cast<VkFormat>(VulkanFrame::GetGuiColorFormat());
    initInfo.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats =
        &colorAttachmentFormat;
    initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    const auto getInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        SDL_Vulkan_GetVkGetInstanceProcAddr());
    if (!getInstanceProcAddr)
    {
        RAD_LOG_GUI(err, "SDL_Vulkan_GetVkGetInstanceProcAddr failed: {}", SDL_GetError());
        return false;
    }

    if (!ImGui_ImplVulkan_LoadFunctions(
            initInfo.ApiVersion,
            [](const char* function_name, void* user_data) -> PFN_vkVoidFunction
            {
                const auto loader = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
                    SDL_Vulkan_GetVkGetInstanceProcAddr());
                return loader(static_cast<VkInstance>(user_data), function_name);
            },
            reinterpret_cast<void*>(initInfo.Instance)))
    {
        RAD_LOG_GUI(err, "ImGui_ImplVulkan_LoadFunctions failed!");
        return false;
    }

    if (!ImGui_ImplVulkan_Init(&initInfo))
    {
        RAD_LOG_GUI(err, "ImGui_ImplVulkan_Init failed!");
        return false;
    }
    return true;
}

void VulkanGuiRenderer::Destroy()
{
    if (VulkanDevice* device = GetDevice())
    {
        device->WaitIdle();
    }
    if (m_imgui)
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        m_imgui = nullptr;
    }
    m_frame = nullptr;
}

SDL_Window* VulkanGuiRenderer::GetRenderWindowHandle() const
{
    return m_window ? m_window->GetHandle() : nullptr;
}

bool VulkanGuiRenderer::GetOutputSize(int* w, int* h)
{
    return m_window && m_window->GetSizeInPixels(w, h);
}

void VulkanGuiRenderer::SetMinImageCount(uint32_t minImageCount)
{
    ImGui_ImplVulkan_SetMinImageCount(minImageCount);
}

void VulkanGuiRenderer::BeginFrame()
{
    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();
}

void VulkanGuiRenderer::Render()
{
    VulkanCommandBuffer* cmd = m_frame ? m_frame->GetCommandBuffer() : nullptr;
    VulkanImage* guiColorImage = GetGuiColorImage();
    VulkanImageView* guiColorImageView = GetGuiColorImageView();
    if (!cmd || !guiColorImage || !guiColorImageView || !m_frame)
    {
        return;
    }

    const ImGuiIO& io = ImGui::GetIO();
    if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f)
    {
        return;
    }

    const vk::Extent2D extent = m_frame->GetExtent();
    cmd->TransitionImageLayout(guiColorImage, vk::ImageLayout::eColorAttachmentOptimal,
                               vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                               vk::AccessFlagBits2::eColorAttachmentWrite);
    const VulkanImageView* colorViews[] = {guiColorImageView};
    const vk::AttachmentLoadOp colorLoadOps[] = {vk::AttachmentLoadOp::eClear};
    const vk::AttachmentStoreOp colorStoreOps[] = {vk::AttachmentStoreOp::eStore};
    vk::ClearValue colorClearValues[1] = {};
    colorClearValues[0].color = vk::ClearColorValue(std::array<float, 4>{0.f, 0.f, 0.f, 0.f});
    const vk::Rect2D renderArea = {{0, 0}, extent};
    cmd->BeginRendering(colorViews, colorLoadOps, colorStoreOps,
                        colorClearValues, nullptr, vk::AttachmentLoadOp::eClear,
                        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear,
                        vk::AttachmentStoreOp::eStore, vk::ClearDepthStencilValue{1.f, 0},
                        &renderArea);

    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(),
                                    static_cast<VkCommandBuffer>(cmd->GetHandle()));

    cmd->EndRendering();

    cmd->TransitionImageLayout(guiColorImage, vk::ImageLayout::eShaderReadOnlyOptimal,
                               vk::PipelineStageFlagBits2::eFragmentShader,
                               vk::AccessFlagBits2::eShaderRead);
}

bool VulkanGuiRenderer::ProcessEvent(const SDL_Event& event)
{
    return ImGui_ImplSDL3_ProcessEvent(&event);
}

} // namespace rad
