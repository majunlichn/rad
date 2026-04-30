#pragma once

#include <rad/Vulkan/VulkanCommon.h>

#include <rad/Common/Result.h>

#include <spirv-tools/libspirv.h>

namespace rad
{

struct SpvError
{
    std::string message;
};

class SpvContext : public RefCounted<SpvContext>
{
public:
    SpvContext();
    ~SpvContext();

    void SetTargetEnv(spv_target_env env);

    Result<void, SpvError> Validate(const uint32_t* binary, size_t wordCount);
    Result<void, SpvError> Validate(Span<uint32_t> binary);

    enum class DisassembleOptions : uint32_t
    {
        None = 0,
        FriendlyNames = 1u << 0,
        Comments = 1u << 1,
        NestedIndent = 1u << 2,
        ReorderBlocks = 1u << 3,
        Default = FriendlyNames | Comments | NestedIndent | ReorderBlocks,
    };

    Result<std::string, SpvError> Disassemble(
        const uint32_t* binary, size_t wordCount,
        DisassembleOptions options = DisassembleOptions::Default);
    Result<std::string, SpvError> Disassemble(Span<uint32_t> binary);

    Result<std::vector<uint32_t>, SpvError> Assemble(std::string_view assembly);

    Result<std::string, SpvError> ValidateAndDisassemble(
        Span<uint32_t> binary, DisassembleOptions options = DisassembleOptions::Default);

private:
    void CreateContext();

    spv_target_env m_targetEnv = SPV_ENV_VULKAN_1_4;
    spv_context m_spvContext = nullptr;

}; // class SpvContext

} // namespace rad
