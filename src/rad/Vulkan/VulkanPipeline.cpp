#include <rad/Vulkan/VulkanPipeline.h>

#include <rad/Vulkan/VulkanDevice.h>
#include <rad/Vulkan/VulkanRenderPass.h>

namespace rad
{

VulkanShaderModule::VulkanShaderModule(Ref<VulkanDevice> device,
                                       const vk::ShaderModuleCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createShaderModule(createInfo, nullptr, GetDispatcher());
}

VulkanShaderModule::~VulkanShaderModule()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyShaderModule(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanShaderModule::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

VulkanPipelineLayout::VulkanPipelineLayout(Ref<VulkanDevice> device,
                                           const vk::PipelineLayoutCreateInfo& createInfo) :
    m_device(std::move(device))
{
    m_handle = m_device->GetHandle().createPipelineLayout(createInfo, nullptr, GetDispatcher());
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyPipelineLayout(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanPipelineLayout::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

VulkanGraphicsPipelineCreateInfo::VulkanGraphicsPipelineCreateInfo()
{
    m_dynamicState.states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    // Even when viewport/scissor are dynamic, Vulkan still requires non-zero counts in the
    // viewport state.
    m_viewportState.viewportCount = 1;
    m_viewportState.scissorCount = 1;
}

VulkanGraphicsPipelineCreateInfo::~VulkanGraphicsPipelineCreateInfo()
{
}

VulkanComputePipelineCreateInfo::VulkanComputePipelineCreateInfo()
{
}

VulkanComputePipelineCreateInfo::~VulkanComputePipelineCreateInfo()
{
}

VulkanPipeline::VulkanPipeline(Ref<VulkanDevice> device) :
    m_device(std::move(device))
{
}

VulkanPipeline::~VulkanPipeline()
{
    if (m_handle)
    {
        m_device->GetHandle().destroyPipeline(m_handle, nullptr, GetDispatcher());
        m_handle = nullptr;
    }
}

const vk::detail::DispatchLoaderDynamic& VulkanPipeline::GetDispatcher() const
{
    return m_device->GetDispatcher();
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(Ref<VulkanDevice> device,
                                               const VulkanGraphicsPipelineCreateInfo& info) :
    VulkanPipeline(std::move(device))
{
    m_info = info;
    assert(info.m_layout);
    assert(info.m_renderPass || !info.m_colorAttachmentFormats.empty() ||
           info.m_depthAttachmentFormat != vk::Format::eUndefined ||
           info.m_stencilAttachmentFormat != vk::Format::eUndefined);

    vk::GraphicsPipelineCreateInfo createInfo = {};
    createInfo.flags = info.m_flags;
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages(info.m_stages.size());
    bool hasTessellationStage = false;
    for (size_t i = 0; i < info.m_stages.size(); ++i)
    {
        shaderStages[i] = info.m_stages[i];
        if ((shaderStages[i].stage & vk::ShaderStageFlagBits::eTessellationControl) ||
            (shaderStages[i].stage & vk::ShaderStageFlagBits::eTessellationEvaluation))
        {
            hasTessellationStage = true;
        }
    }
    createInfo.setStages(shaderStages);
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.setVertexBindingDescriptions(info.m_vertexInputState.bindings);
    vertexInputStateCreateInfo.setVertexAttributeDescriptions(info.m_vertexInputState.attributes);
    createInfo.pVertexInputState = &vertexInputStateCreateInfo;
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.topology = info.m_inputAssemblyState.topology;
    inputAssemblyStateCreateInfo.primitiveRestartEnable =
        info.m_inputAssemblyState.primitiveRestartEnable;
    createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    vk::PipelineTessellationStateCreateInfo tessellationStateCreateInfo = {};
    if (hasTessellationStage)
    {
        tessellationStateCreateInfo.patchControlPoints =
            info.m_tessellationState.patchControlPoints;
        createInfo.pTessellationState = &tessellationStateCreateInfo;
    }
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.viewportCount = info.m_viewportState.viewportCount;
    if (viewportStateCreateInfo.viewportCount == 0)
    {
        viewportStateCreateInfo.viewportCount =
            info.m_viewportState.viewports.empty()
                ? 1u
                : uint32_t(info.m_viewportState.viewports.size());
    }
    viewportStateCreateInfo.pViewports =
        info.m_viewportState.viewports.size() > 0 ? info.m_viewportState.viewports.data() : nullptr;
    viewportStateCreateInfo.scissorCount = info.m_viewportState.scissorCount;
    if (viewportStateCreateInfo.scissorCount == 0)
    {
        viewportStateCreateInfo.scissorCount = info.m_viewportState.scissors.empty()
                                                   ? 1u
                                                   : uint32_t(info.m_viewportState.scissors.size());
    }
    viewportStateCreateInfo.pScissors =
        info.m_viewportState.scissors.size() > 0 ? info.m_viewportState.scissors.data() : nullptr;
    createInfo.pViewportState = &viewportStateCreateInfo;
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.depthClampEnable = info.m_rasterizationState.depthClampEnable;
    rasterizationStateCreateInfo.rasterizerDiscardEnable =
        info.m_rasterizationState.rasterizerDiscardEnable;
    rasterizationStateCreateInfo.polygonMode = info.m_rasterizationState.polygonMode;
    rasterizationStateCreateInfo.cullMode = info.m_rasterizationState.cullMode;
    rasterizationStateCreateInfo.frontFace = info.m_rasterizationState.frontFace;
    rasterizationStateCreateInfo.depthBiasEnable = info.m_rasterizationState.depthBiasEnable;
    rasterizationStateCreateInfo.depthBiasConstantFactor =
        info.m_rasterizationState.depthBiasConstantFactor;
    rasterizationStateCreateInfo.depthBiasClamp = info.m_rasterizationState.depthBiasClamp;
    rasterizationStateCreateInfo.depthBiasSlopeFactor =
        info.m_rasterizationState.depthBiasSlopeFactor;
    rasterizationStateCreateInfo.lineWidth = info.m_rasterizationState.lineWidth;
    createInfo.pRasterizationState = &rasterizationStateCreateInfo;
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.rasterizationSamples = info.m_multisampleState.rasterizationSamples;
    multisampleStateCreateInfo.sampleShadingEnable = info.m_multisampleState.sampleShadingEnable;
    multisampleStateCreateInfo.minSampleShading = info.m_multisampleState.minSampleShading;
    multisampleStateCreateInfo.pSampleMask = info.m_multisampleState.sampleMask.size() > 0
                                                 ? info.m_multisampleState.sampleMask.data()
                                                 : nullptr;
    multisampleStateCreateInfo.alphaToCoverageEnable =
        info.m_multisampleState.alphaToCoverageEnable;
    multisampleStateCreateInfo.alphaToOneEnable = info.m_multisampleState.alphaToOneEnable;
    createInfo.pMultisampleState = &multisampleStateCreateInfo;
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.depthTestEnable = info.m_depthStencilState.depthTestEnable;
    depthStencilStateCreateInfo.depthWriteEnable = info.m_depthStencilState.depthWriteEnable;
    depthStencilStateCreateInfo.depthCompareOp = info.m_depthStencilState.depthCompareOp;
    depthStencilStateCreateInfo.depthBoundsTestEnable =
        info.m_depthStencilState.depthBoundsTestEnable;
    depthStencilStateCreateInfo.stencilTestEnable = info.m_depthStencilState.stencilTestEnable;
    depthStencilStateCreateInfo.front = info.m_depthStencilState.front;
    depthStencilStateCreateInfo.back = info.m_depthStencilState.back;
    depthStencilStateCreateInfo.minDepthBounds = info.m_depthStencilState.minDepthBounds;
    depthStencilStateCreateInfo.maxDepthBounds = info.m_depthStencilState.maxDepthBounds;
    createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.logicOpEnable = info.m_colorBlendState.logicOpEnable;
    colorBlendStateCreateInfo.logicOp = info.m_colorBlendState.logicOp;
    colorBlendStateCreateInfo.setAttachments(info.m_colorBlendState.attachments);
    colorBlendStateCreateInfo.blendConstants[0] = info.m_colorBlendState.blendConstants[0];
    colorBlendStateCreateInfo.blendConstants[1] = info.m_colorBlendState.blendConstants[1];
    colorBlendStateCreateInfo.blendConstants[2] = info.m_colorBlendState.blendConstants[2];
    colorBlendStateCreateInfo.blendConstants[3] = info.m_colorBlendState.blendConstants[3];
    createInfo.pColorBlendState = &colorBlendStateCreateInfo;
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.setDynamicStates(info.m_dynamicState.states);
    createInfo.pDynamicState = &dynamicStateCreateInfo;
    createInfo.layout = info.m_layout->GetHandle();
    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo = {};
    if (info.m_renderPass)
    {
        createInfo.renderPass = info.m_renderPass->GetHandle();
        createInfo.subpass = info.m_subpass;
    }
    else
    {
        pipelineRenderingCreateInfo.setColorAttachmentFormats(info.m_colorAttachmentFormats);
        pipelineRenderingCreateInfo.depthAttachmentFormat = info.m_depthAttachmentFormat;
        pipelineRenderingCreateInfo.stencilAttachmentFormat = info.m_stencilAttachmentFormat;
        pipelineRenderingCreateInfo.viewMask = info.m_viewMask;
        createInfo.pNext = &pipelineRenderingCreateInfo;
        createInfo.renderPass = nullptr;
        createInfo.subpass = 0;
    }
    auto [result, handle] =
        m_device->GetHandle().createGraphicsPipeline(nullptr, createInfo, nullptr, GetDispatcher());
    VK_CHECK(result);
    if (result == vk::Result::eSuccess)
    {
        m_handle = handle;
        m_bindPoint = vk::PipelineBindPoint::eGraphics;
    }
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
}

VulkanComputePipeline::VulkanComputePipeline(Ref<VulkanDevice> device,
                                             const VulkanComputePipelineCreateInfo& info) :
    VulkanPipeline(std::move(device))
{
    m_info = info;
    vk::ComputePipelineCreateInfo createInfo = {};
    createInfo.flags = info.m_flags;
    createInfo.stage = info.m_stage;
    createInfo.layout = info.m_layout->GetHandle();
    auto [result, handle] =
        m_device->GetHandle().createComputePipeline(nullptr, createInfo, nullptr, GetDispatcher());
    VK_CHECK(result);
    if (result == vk::Result::eSuccess)
    {
        m_handle = handle;
        m_bindPoint = vk::PipelineBindPoint::eCompute;
    }
}

VulkanComputePipeline::~VulkanComputePipeline()
{
}

} // namespace rad
