#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/VulkanGuiComposition.h>

#include <rad/Gui/VulkanFrame.h>
#include <rad/Gui/VulkanGuiRenderer.h>
#include <rad/Gui/VulkanWindow.h>

#include <rad/Vulkan/VulkanShader.h>

#include <stdexcept>
#include <vector>

namespace rad
{
namespace
{

// Fragment push constants for the composition pass. Layout must match GuiComposition.frag.
struct GuiCompositionPushConstants
{
    uint32_t colorSpace; // VkColorSpaceKHR for the active swapchain (GuiComposition.frag).
    float referenceWhiteNits; // SDR white luminance target for HDR encodings.
};

Ref<VulkanSampler> CreateCompositionSampler(VulkanDevice* device, vk::Filter filter)
{
    if (!device)
    {
        return nullptr;
    }

    vk::SamplerCreateInfo info;
    info.magFilter = filter;
    info.minFilter = filter;
    info.mipmapMode = vk::SamplerMipmapMode::eNearest;
    info.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    info.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    info.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    info.minLod = 0.0f;
    info.maxLod = 0.0f;
    return device->CreateSampler(info);
}

} // namespace

VulkanGuiComposition::VulkanGuiComposition(VulkanFrame* frame) :
    m_frame(frame)
{
    VulkanWindow* window = frame ? frame->GetWindow() : nullptr;
    m_device = window ? window->GetVulkanDevice() : nullptr;
    if (!m_frame || !m_device || !m_frame->GetSwapchain())
    {
        throw std::runtime_error("VulkanGuiComposition requires an initialized VulkanFrame");
    }

    if (!OnSwapchainChanged(m_frame->GetFrameLag(), m_frame->GetSwapchainColorFormat()))
    {
        Shutdown();
        throw std::runtime_error(
            "VulkanGuiComposition failed to create swapchain-dependent resources");
    }
}

VulkanGuiComposition::~VulkanGuiComposition()
{
    if (m_device)
    {
        m_device->WaitIdle();
    }
    Shutdown();
}

void VulkanGuiComposition::Shutdown()
{
    DestroyDescriptorBindings();
    DestroyPipeline();
    m_sceneColorSampler.reset();
    m_guiColorSampler.reset();
}

bool VulkanGuiComposition::EnsureSamplers()
{
    if (m_sceneColorSampler && m_guiColorSampler)
    {
        return true;
    }

    if (!m_sceneColorSampler)
    {
        m_sceneColorSampler =
            CreateCompositionSampler(m_device, vk::Filter::eLinear);
    }
    if (!m_guiColorSampler)
    {
        m_guiColorSampler =
            CreateCompositionSampler(m_device, vk::Filter::eNearest);
    }
    return m_sceneColorSampler != nullptr && m_guiColorSampler != nullptr;
}

bool VulkanGuiComposition::CreatePipeline(vk::Format swapchainColorFormat)
{
    if (swapchainColorFormat == vk::Format::eUndefined)
    {
        return false;
    }

    vk::DescriptorSetLayoutBinding bindings[2] = {};
    bindings[0].binding = 0;
    bindings[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[0].descriptorCount = 1;
    bindings[0].stageFlags = vk::ShaderStageFlagBits::eFragment;
    bindings[1].binding = 1;
    bindings[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    bindings[1].descriptorCount = 1;
    bindings[1].stageFlags = vk::ShaderStageFlagBits::eFragment;

    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = 2;
    layoutInfo.pBindings = bindings;
    m_descSetLayout = m_device->CreateDescriptorSetLayout(layoutInfo);
    if (!m_descSetLayout)
    {
        return false;
    }

    vk::PushConstantRange pushConstantRange = {};
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eFragment;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(GuiCompositionPushConstants);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
    const vk::DescriptorSetLayout setLayoutHandle = m_descSetLayout->GetHandle();
    pipelineLayoutInfo.setSetLayouts(setLayoutHandle);
    pipelineLayoutInfo.setPushConstantRanges(pushConstantRange);
    m_pipelineLayout = m_device->CreatePipelineLayout(pipelineLayoutInfo);
    if (!m_pipelineLayout)
    {
        return false;
    }

    const std::vector<uint32_t> vertSpv = LoadShaderSpv("GuiComposition.vert.spv");
    const std::vector<uint32_t> fragSpv = LoadShaderSpv("GuiComposition.frag.spv");
    if (vertSpv.empty())
    {
        RAD_LOG_GUI(err, "VulkanGuiComposition failed to load GuiComposition.vert.spv");
        return false;
    }
    if (fragSpv.empty())
    {
        RAD_LOG_GUI(err, "VulkanGuiComposition failed to load GuiComposition.frag.spv");
        return false;
    }

    vk::ShaderModuleCreateInfo vertModuleInfo;
    vertModuleInfo.codeSize = vertSpv.size() * sizeof(uint32_t);
    vertModuleInfo.pCode = vertSpv.data();
    Ref<VulkanShaderModule> vertModule = m_device->CreateShaderModule(vertModuleInfo);

    vk::ShaderModuleCreateInfo fragModuleInfo;
    fragModuleInfo.codeSize = fragSpv.size() * sizeof(uint32_t);
    fragModuleInfo.pCode = fragSpv.data();
    Ref<VulkanShaderModule> fragModule = m_device->CreateShaderModule(fragModuleInfo);
    if (!vertModule || !fragModule)
    {
        return false;
    }

    Ref<VulkanGraphicsPipelineCreateInfo> pipelineInfo = RAD_NEW VulkanGraphicsPipelineCreateInfo();
    pipelineInfo->m_layout = m_pipelineLayout;
    pipelineInfo->m_colorAttachmentFormats = {swapchainColorFormat};

    VulkanShaderStageInfo vertStage;
    vertStage.m_stage = vk::ShaderStageFlagBits::eVertex;
    vertStage.m_module = vertModule;
    vertStage.m_entryPoint = "main";
    VulkanShaderStageInfo fragStage;
    fragStage.m_stage = vk::ShaderStageFlagBits::eFragment;
    fragStage.m_module = fragModule;
    fragStage.m_entryPoint = "main";
    pipelineInfo->m_stages = {vertStage, fragStage};

    pipelineInfo->m_inputAssemblyState.topology = vk::PrimitiveTopology::eTriangleList;
    pipelineInfo->m_rasterizationState.cullMode = vk::CullModeFlagBits::eNone;
    pipelineInfo->m_dynamicState.states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    vk::PipelineColorBlendAttachmentState blendAttachment = {};
    blendAttachment.colorWriteMask =
        vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    pipelineInfo->m_colorBlendState.attachments = {blendAttachment};

    m_pipeline = m_device->CreateGraphicsPipeline(*pipelineInfo);
    return m_pipeline != nullptr;
}

void VulkanGuiComposition::DestroyPipeline()
{
    m_pipeline.reset();
    m_pipelineLayout.reset();
    m_descSetLayout.reset();
}

bool VulkanGuiComposition::CreateDescriptorBindings(uint32_t frameLag)
{
    if (frameLag == 0 || !m_descSetLayout || !EnsureSamplers())
    {
        return false;
    }

    vk::DescriptorPoolSize poolSize;
    poolSize.type = vk::DescriptorType::eCombinedImageSampler;
    poolSize.descriptorCount = 2 * frameLag;

    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.maxSets = frameLag;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    m_descPool = m_device->CreateDescriptorPool(poolInfo);
    if (!m_descPool)
    {
        return false;
    }

    const std::vector<vk::DescriptorSetLayout> layouts(frameLag, m_descSetLayout->GetHandle());
    m_descSets = m_descPool->Allocate(layouts);
    if (m_descSets.size() != frameLag)
    {
        return false;
    }

    for (uint32_t i = 0; i < frameLag; ++i)
    {
        VulkanImageView* sceneColorImageView = m_frame->GetSceneColorImageView(i);
        VulkanImageView* guiColorImageView = m_frame->GetGuiColorImageView(i);
        if (!sceneColorImageView || !guiColorImageView)
        {
            return false;
        }
        UpdateResourceBindings(m_descSets[i].get(), sceneColorImageView, guiColorImageView);
    }
    return true;
}

void VulkanGuiComposition::DestroyDescriptorBindings()
{
    m_descSets.clear();
    m_descPool.reset();
}

void VulkanGuiComposition::UpdateResourceBindings(VulkanDescriptorSet* descSet,
                                                  VulkanImageView* sceneColorImageView,
                                                  VulkanImageView* guiColorImageView)
{
    if (!descSet || !m_sceneColorSampler || !m_guiColorSampler || !sceneColorImageView ||
        !guiColorImageView)
    {
        return;
    }

    vk::DescriptorImageInfo sceneColorImageInfo = {};
    sceneColorImageInfo.sampler = m_sceneColorSampler->GetHandle();
    sceneColorImageInfo.imageView = sceneColorImageView->GetHandle();
    sceneColorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    vk::DescriptorImageInfo guiColorImageInfo = {};
    guiColorImageInfo.sampler = m_guiColorSampler->GetHandle();
    guiColorImageInfo.imageView = guiColorImageView->GetHandle();
    guiColorImageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    vk::WriteDescriptorSet writes[2] = {};
    writes[0].dstSet = descSet->GetHandle();
    writes[0].dstBinding = 0;
    writes[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writes[0].descriptorCount = 1;
    writes[0].pImageInfo = &sceneColorImageInfo;
    writes[1].dstSet = descSet->GetHandle();
    writes[1].dstBinding = 1;
    writes[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
    writes[1].descriptorCount = 1;
    writes[1].pImageInfo = &guiColorImageInfo;
    m_device->UpdateDescriptorSets(writes, {});
}

bool VulkanGuiComposition::OnSwapchainChanged(uint32_t frameLag, vk::Format swapchainColorFormat)
{
    if (frameLag == 0 || swapchainColorFormat == vk::Format::eUndefined)
    {
        return false;
    }

    m_device->WaitIdle();

    const bool swapchainFormatChanged = swapchainColorFormat != m_swapchainColorFormat;
    if (swapchainFormatChanged)
    {
        DestroyDescriptorBindings();
        DestroyPipeline();
        if (!CreatePipeline(swapchainColorFormat))
        {
            RAD_LOG_GUI(err, "VulkanGuiComposition::OnSwapchainChanged failed to create pipeline");
            return false;
        }
        m_swapchainColorFormat = swapchainColorFormat;
    }
    else
    {
        DestroyDescriptorBindings();
    }

    if (!CreateDescriptorBindings(frameLag))
    {
        RAD_LOG_GUI(err,
                    "VulkanGuiComposition::OnSwapchainChanged failed to create descriptor bindings");
        if (swapchainFormatChanged)
        {
            DestroyDescriptorBindings();
            DestroyPipeline();
            m_swapchainColorFormat = vk::Format::eUndefined;
        }
        return false;
    }

    return true;
}

void VulkanGuiComposition::Record()
{
    VulkanCommandBuffer* cmd = m_frame ? m_frame->GetCommandBuffer() : nullptr;
    VulkanImageView* guiColorImageView = m_frame ? m_frame->GetGuiColorImageView() : nullptr;
    if (!cmd)
    {
        RAD_LOG_GUI(warn, "VulkanGuiComposition::Record: no command buffer");
        return;
    }
    if (!m_frame)
    {
        RAD_LOG_GUI(warn, "VulkanGuiComposition::Record: no frame");
        return;
    }
    if (!m_pipeline || !m_pipelineLayout)
    {
        RAD_LOG_GUI(warn, "VulkanGuiComposition::Record: pipeline not ready");
        return;
    }
    if (!m_sceneColorSampler || !m_guiColorSampler)
    {
        RAD_LOG_GUI(warn, "VulkanGuiComposition::Record: samplers not ready");
        return;
    }
    if (!guiColorImageView)
    {
        RAD_LOG_GUI(warn, "VulkanGuiComposition::Record: no GUI color image view");
        return;
    }

    const uint32_t frameIndex = m_frame->GetFrameIndex();
    VulkanImage* sceneColorImage = m_frame->GetSceneColorImage();
    VulkanImage* swapchainImage = m_frame->GetSwapchainImage();
    VulkanImageView* swapchainImageView = m_frame->GetSwapchainImageView();
    if (!sceneColorImage)
    {
        RAD_LOG_GUI(warn, "VulkanGuiComposition::Record: no scene color image");
        return;
    }
    if (!swapchainImage || !swapchainImageView)
    {
        RAD_LOG_GUI(warn, "VulkanGuiComposition::Record: no swapchain image");
        return;
    }
    if (frameIndex >= m_descSets.size())
    {
        RAD_LOG_GUI(warn, "VulkanGuiComposition::Record: frame index {} out of range ({} sets)",
                    frameIndex, m_descSets.size());
        return;
    }

    VulkanDescriptorSet* descSet = m_descSets[frameIndex].get();

    cmd->TransitionImageLayout(sceneColorImage, vk::ImageLayout::eShaderReadOnlyOptimal,
                               vk::PipelineStageFlagBits2::eFragmentShader,
                               vk::AccessFlagBits2::eShaderRead);

    cmd->TransitionImageLayout(swapchainImage, vk::ImageLayout::eColorAttachmentOptimal,
                               vk::PipelineStageFlagBits2::eColorAttachmentOutput,
                               vk::AccessFlagBits2::eColorAttachmentWrite);

    const vk::Extent2D extent = m_frame->GetExtent();
    const VulkanImageView* colorViews[] = {swapchainImageView};
    const vk::AttachmentLoadOp colorLoadOps[] = {vk::AttachmentLoadOp::eDontCare};
    const vk::AttachmentStoreOp colorStoreOps[] = {vk::AttachmentStoreOp::eStore};
    const vk::ClearValue colorClearValues[1] = {};
    const vk::Rect2D renderArea = {{0, 0}, extent};
    cmd->BeginRendering(MakeSpan(colorViews), MakeSpan(colorLoadOps), MakeSpan(colorStoreOps),
                        MakeSpan(colorClearValues), nullptr, vk::AttachmentLoadOp::eClear,
                        vk::AttachmentStoreOp::eStore, vk::AttachmentLoadOp::eClear,
                        vk::AttachmentStoreOp::eStore, vk::ClearDepthStencilValue{1.f, 0},
                        &renderArea);

    cmd->BindPipeline(m_pipeline.get());
    cmd->BindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_pipelineLayout.get(), 0, {descSet},
                            {});

    const GuiCompositionPushConstants pushConstants = {
        static_cast<uint32_t>(m_frame->GetSwapchainColorSpace()),
        m_frame->GetHdrReferenceWhiteNits(),
    };
    cmd->PushConstants(m_pipelineLayout.get(), vk::ShaderStageFlagBits::eFragment, 0,
                       sizeof(pushConstants), &pushConstants);

    const vk::Viewport viewport = {
        0.f, 0.f, static_cast<float>(extent.width), static_cast<float>(extent.height), 0.f, 1.f};
    const vk::Rect2D scissor = {{0, 0}, extent};
    cmd->SetViewportWithCount({viewport});
    cmd->SetScissorWithCount({scissor});

    cmd->Draw(3, 1, 0, 0);

    cmd->EndRendering();

    cmd->TransitionImageLayout(
        swapchainImage, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe, vk::AccessFlagBits2::eColorAttachmentWrite, {});
}

} // namespace rad
