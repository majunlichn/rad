#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Vulkan/VulkanCore.h>

#include <vector>

namespace rad
{

class VulkanFrame;

// GUI composition/output pass. Combines the scene and GUI layers onto the swapchain image and applies
// the display transfer function (sRGB OETF / PQ / scRGB scaling) from VulkanFrame.
class VulkanGuiComposition : public RefCounted<VulkanGuiComposition>
{
public:
    explicit VulkanGuiComposition(VulkanFrame* frame);
    ~VulkanGuiComposition();

    VulkanGuiComposition(const VulkanGuiComposition&) = delete;
    VulkanGuiComposition& operator=(const VulkanGuiComposition&) = delete;

    VulkanFrame* GetFrame() const { return m_frame; }

    // Rebuilds the pipeline and per-frame descriptor bindings when swapchain format or in-flight
    // frame count changes. Called from VulkanFrame after swapchain-dependent resources are recreated.
    bool OnSwapchainChanged(uint32_t frameLag, vk::Format swapchainColorFormat);

    // Composites the GUI color image over the scene onto the acquired swapchain image. Called from
    // VulkanFrame::EndFrame() after the GUI pass.
    void Record();

private:
    void Shutdown();
    bool EnsureSamplers();
    bool CreatePipeline(vk::Format swapchainColorFormat);
    void DestroyPipeline();
    bool CreateDescriptorBindings(uint32_t frameLag);
    void DestroyDescriptorBindings();
    void UpdateResourceBindings(VulkanDescriptorSet* descSet, VulkanImageView* sceneColorImageView,
                                VulkanImageView* guiColorImageView);

    VulkanFrame* m_frame = nullptr;
    VulkanDevice* m_device = nullptr;

    vk::Format m_swapchainColorFormat = vk::Format::eUndefined;

    std::vector<Ref<VulkanDescriptorSet>> m_descSets;

    // Scene: linear filtering for upscaling/downscaling when render resolution != composite size.
    Ref<VulkanSampler> m_sceneColorSampler;
    // GUI: nearest filtering; targets match the swapchain extent (1:1 sample).
    Ref<VulkanSampler> m_guiColorSampler;
    Ref<VulkanDescriptorSetLayout> m_descSetLayout;
    Ref<VulkanDescriptorPool> m_descPool;
    Ref<VulkanPipelineLayout> m_pipelineLayout;
    Ref<VulkanGraphicsPipeline> m_pipeline;

}; // class VulkanGuiComposition

} // namespace rad
