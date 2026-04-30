#pragma once

#include <rad/Vulkan/VulkanCommon.h>

#include <rad/Common/Result.h>

#include <shaderc/shaderc.h>

namespace shaderc
{
class Compiler;
}

namespace rad
{

struct GLSLMacro
{
    GLSLMacro() {}

    GLSLMacro(std::string_view name) { m_name = name; }
    GLSLMacro(std::string_view name, std::string_view value)
    {
        this->m_name = name;
        this->m_value = value;
    }

    std::string m_name;
    std::string m_value;

}; // struct GLSLMacro

enum class GLSLCompileOptLevel
{
    Zero,        // no optimization
    Size,        // optimize towards reducing code size
    Performance, // optimize towards performance
};

struct GLSLCompileError
{
    std::string message;
};

class GLSLCompiler : public RefCounted<GLSLCompiler>
{
public:
    GLSLCompiler();
    ~GLSLCompiler();

    void SetTargetVulkanVersion(shaderc_env_version version);
    void SetTargetSpirvVersion(shaderc_spirv_version version);

    void SetIncludeDirs(const std::vector<std::string>& includeDirs);
    void AddIncludeDir(std::string includeDir);

    Result<std::string, GLSLCompileError> Preprocess(vk::ShaderStageFlagBits stage,
                                                     const std::string& fileName,
                                                     const std::string& source,
                                                     const std::string& entryPoint = "main",
                                                     Span<GLSLMacro> macros = {});
    Result<std::vector<uint32_t>, GLSLCompileError> CompileToSpv(
        vk::ShaderStageFlagBits stage, const std::string& fileName, const std::string& source,
        const std::string& entryPoint = "main", Span<GLSLMacro> macros = {},
        GLSLCompileOptLevel opt = GLSLCompileOptLevel::Zero);

private:
    std::unique_ptr<shaderc::Compiler> m_compiler;
    std::vector<std::string> m_includeDirs;
    shaderc_env_version m_targetVulkanVersion = shaderc_env_version_vulkan_1_4;
    shaderc_spirv_version m_targetSpirvVersion = shaderc_spirv_version_1_6;

}; // class GLSLCompiler

} // namespace rad
