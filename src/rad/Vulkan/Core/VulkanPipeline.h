#pragma once

#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

class VulkanShaderModule : public RefCounted<VulkanShaderModule>
{
public:
    VulkanShaderModule(Ref<VulkanDevice> device, const vk::ShaderModuleCreateInfo& createInfo);
    ~VulkanShaderModule();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::ShaderModule& GetHandle() const { return m_handle; }

private:
    Ref<VulkanDevice> m_device;
    vk::ShaderModule m_handle = nullptr;

}; // class VulkanShaderModule

struct VulkanShaderStageInfo : public RefCounted<VulkanShaderStageInfo>
{
public:
    VulkanShaderStageInfo() = default;
    ~VulkanShaderStageInfo() = default;

    template <typename T>
    void SetSpecialization(uint32_t constantID, const T& value)
    {
        size_t offset = m_specData.size();
        m_specData.resize(m_specData.size() + sizeof(value));
        std::memcpy(m_specData.data() + offset, &value, sizeof(value));
        m_specMapEntries.emplace_back(constantID, static_cast<uint32_t>(offset), sizeof(T));

        m_specInfo.mapEntryCount = static_cast<uint32_t>(m_specMapEntries.size());
        m_specInfo.pMapEntries = m_specMapEntries.data();
        m_specInfo.dataSize = m_specData.size();
        m_specInfo.pData = m_specData.data();
    }

    operator vk::PipelineShaderStageCreateInfo() const
    {
        assert(m_module);
        assert(!m_entryPoint.empty());
        vk::PipelineShaderStageCreateInfo createInfo = {};
        createInfo.flags = m_flags;
        createInfo.stage = m_stage;
        createInfo.module = m_module->GetHandle();
        createInfo.pName = m_entryPoint.c_str();
        createInfo.pSpecializationInfo = nullptr;
        if (!m_specMapEntries.empty() && !m_specData.empty())
        {
            createInfo.pSpecializationInfo = &m_specInfo;
        }
        return createInfo;
    }

    vk::PipelineShaderStageCreateFlags m_flags = {};
    vk::ShaderStageFlagBits m_stage = vk::ShaderStageFlagBits::eCompute;
    Ref<VulkanShaderModule> m_module;
    std::string m_entryPoint;
    vk::SpecializationInfo m_specInfo;
    std::vector<vk::SpecializationMapEntry> m_specMapEntries;
    std::vector<uint8_t> m_specData;

}; // class VulkanShaderStageInfo

class VulkanPipelineLayout : public RefCounted<VulkanPipelineLayout>
{
public:
    VulkanPipelineLayout(Ref<VulkanDevice> device, const vk::PipelineLayoutCreateInfo& createInfo);
    ~VulkanPipelineLayout();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::PipelineLayout& GetHandle() const { return m_handle; }

private:
    Ref<VulkanDevice> m_device;
    vk::PipelineLayout m_handle = nullptr;

}; // class VulkanPipelineLayout

struct VulkanGraphicsPipelineCreateInfo : public RefCounted<VulkanGraphicsPipelineCreateInfo>
{
    VulkanGraphicsPipelineCreateInfo();
    ~VulkanGraphicsPipelineCreateInfo();

    vk::PipelineCreateFlags m_flags = {};
    std::vector<VulkanShaderStageInfo> m_stages;
    struct VertexInputState
    {
        std::vector<vk::VertexInputBindingDescription> bindings;
        std::vector<vk::VertexInputAttributeDescription> attributes;
    } m_vertexInputState;
    struct InputAssemblyState
    {
        vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList;
        vk::Bool32 primitiveRestartEnable = VK_FALSE;
    } m_inputAssemblyState;
    struct TessellationState
    {
        uint32_t patchControlPoints = 0;
    } m_tessellationState;
    struct ViewportState
    {
        uint32_t viewportCount = 0;
        std::vector<vk::Viewport> viewports;
        uint32_t scissorCount = 0;
        std::vector<vk::Rect2D> scissors;
    } m_viewportState;
    struct RasterizationState
    {
        vk::Bool32 depthClampEnable = VK_FALSE;
        vk::Bool32 rasterizerDiscardEnable = VK_FALSE;
        vk::PolygonMode polygonMode = vk::PolygonMode::eFill;
        vk::CullModeFlags cullMode = {};
        vk::FrontFace frontFace = vk::FrontFace::eCounterClockwise;
        vk::Bool32 depthBiasEnable = VK_FALSE;
        float depthBiasConstantFactor = 0.0f;
        float depthBiasClamp = 0.0f;
        float depthBiasSlopeFactor = 0.0f;
        float lineWidth = 1.0f;
    } m_rasterizationState;
    struct MultisampleState
    {
        vk::SampleCountFlagBits rasterizationSamples = vk::SampleCountFlagBits::e1;
        vk::Bool32 sampleShadingEnable = VK_FALSE;
        float minSampleShading = 1.0f;
        std::vector<vk::SampleMask> sampleMask;
        vk::Bool32 alphaToCoverageEnable = VK_FALSE;
        vk::Bool32 alphaToOneEnable = VK_FALSE;
    } m_multisampleState;
    struct DepthStencilState
    {
        vk::Bool32 depthTestEnable = VK_FALSE;
        vk::Bool32 depthWriteEnable = VK_FALSE;
        vk::CompareOp depthCompareOp = vk::CompareOp::eNever;
        vk::Bool32 depthBoundsTestEnable = VK_FALSE;
        vk::Bool32 stencilTestEnable = VK_FALSE;
        vk::StencilOpState front = {};
        vk::StencilOpState back = {};
        float minDepthBounds = 0.0f;
        float maxDepthBounds = 1.0f;
    } m_depthStencilState;
    struct ColorBlendState
    {
        vk::Bool32 logicOpEnable = VK_FALSE;
        vk::LogicOp logicOp = vk::LogicOp::eCopy;
        std::vector<vk::PipelineColorBlendAttachmentState> attachments;
        float blendConstants[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    } m_colorBlendState;
    struct DynamicState
    {
        std::vector<vk::DynamicState> states;
    } m_dynamicState;
    Ref<VulkanPipelineLayout> m_layout;
    Ref<VulkanRenderPass> m_renderPass;
    uint32_t m_subpass = 0;

    // Dynamic Rendering (VK_KHR_dynamic_rendering / Vulkan 1.3 core).
    // If m_renderPass is null, the pipeline will be created for dynamic rendering and
    // these attachment formats will be used to populate vk::PipelineRenderingCreateInfo.
    std::vector<vk::Format> m_colorAttachmentFormats;
    vk::Format m_depthAttachmentFormat = vk::Format::eUndefined;
    vk::Format m_stencilAttachmentFormat = vk::Format::eUndefined;
    uint32_t m_viewMask = 0;

}; // struct VulkanGraphicsPipelineCreateInfo

struct VulkanComputePipelineCreateInfo : public RefCounted<VulkanComputePipelineCreateInfo>
{
    VulkanComputePipelineCreateInfo();
    ~VulkanComputePipelineCreateInfo();

    vk::PipelineCreateFlags m_flags = {};
    VulkanShaderStageInfo m_stage;
    Ref<VulkanPipelineLayout> m_layout;

}; // struct VulkanComputePipelineCreateInfo

class VulkanPipeline : public RefCounted<VulkanPipeline>
{
public:
    VulkanPipeline(Ref<VulkanDevice> device);
    virtual ~VulkanPipeline();

    VulkanDevice* GetDevice() const { return m_device.get(); }
    const vk::detail::DispatchLoaderDynamic& GetDispatcher() const;
    const vk::Pipeline& GetHandle() const { return m_handle; }

    vk::PipelineBindPoint GetBindPoint() const { return m_bindPoint; }

protected:
    Ref<VulkanDevice> m_device;
    vk::Pipeline m_handle = nullptr;
    vk::PipelineBindPoint m_bindPoint = vk::PipelineBindPoint::eGraphics;
}; // class VulkanPipeline

class VulkanGraphicsPipeline : public VulkanPipeline
{
public:
    VulkanGraphicsPipeline(Ref<VulkanDevice> device, const VulkanGraphicsPipelineCreateInfo& info);
    ~VulkanGraphicsPipeline();

private:
    VulkanGraphicsPipelineCreateInfo m_info;

}; // class VulkanGraphicsPipeline

class VulkanComputePipeline : public VulkanPipeline
{
public:
    VulkanComputePipeline(Ref<VulkanDevice> device, const VulkanComputePipelineCreateInfo& info);
    ~VulkanComputePipeline();

private:
    VulkanComputePipelineCreateInfo m_info;

}; // class VulkanComputePipeline

} // namespace rad
