#include <rad/Common/MemoryDebug.h>

#include <rad/Vulkan/GLSLCompiler.h>

#include <shaderc/shaderc.hpp>

#include "internal/shaderc_util.h"

namespace
{

shaderc_shader_kind ToShaderKind(vk::ShaderStageFlagBits stage)
{
    switch (stage)
    {
    case vk::ShaderStageFlagBits::eVertex:
        return shaderc_vertex_shader;
    case vk::ShaderStageFlagBits::eTessellationControl:
        return shaderc_tess_control_shader;
    case vk::ShaderStageFlagBits::eTessellationEvaluation:
        return shaderc_tess_evaluation_shader;
    case vk::ShaderStageFlagBits::eGeometry:
        return shaderc_geometry_shader;
    case vk::ShaderStageFlagBits::eFragment:
        return shaderc_fragment_shader;
    case vk::ShaderStageFlagBits::eCompute:
        return shaderc_compute_shader;
    case vk::ShaderStageFlagBits::eRaygenKHR:
        return shaderc_raygen_shader;
    case vk::ShaderStageFlagBits::eAnyHitKHR:
        return shaderc_anyhit_shader;
    case vk::ShaderStageFlagBits::eClosestHitKHR:
        return shaderc_closesthit_shader;
    case vk::ShaderStageFlagBits::eMissKHR:
        return shaderc_miss_shader;
    case vk::ShaderStageFlagBits::eIntersectionKHR:
        return shaderc_intersection_shader;
    case vk::ShaderStageFlagBits::eCallableKHR:
        return shaderc_callable_shader;
    case vk::ShaderStageFlagBits::eTaskEXT:
        return shaderc_task_shader;
    case vk::ShaderStageFlagBits::eMeshEXT:
        return shaderc_mesh_shader;
    }
    VK_LOG(err, "shaderc: unsupported shader stage {}", vk::to_string(stage));
    RAD_UNREACHABLE();
}

// https://github.com/google/shaderc/blob/main/glslc/src/file_includer.h
// https://github.com/google/shaderc/blob/main/glslc/src/file_includer.cc
// An includer for files implementing shaderc's includer interface. It responds
// to the file including query from the compiler with the full path and content
// of the file to be included. In the case that the file is not found or cannot
// be opened, the full path field of in the response will point to an empty
// string, and error message will be passed to the content field.
// This class provides the basic thread-safety guarantee.
class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
{
public:
    explicit ShaderIncluder(const shaderc_util::FileFinder* file_finder) :
        file_finder_(*file_finder)
    {
    }

    ~ShaderIncluder() override;

    // Resolves a requested source file of a given type from a requesting
    // source into a shaderc_include_result whose contents will remain valid
    // until it's released.
    shaderc_include_result* GetInclude(const char* requested_source, shaderc_include_type type,
                                       const char* requesting_source,
                                       size_t include_depth) override;
    // Releases an include result.
    void ReleaseInclude(shaderc_include_result* include_result) override;

    // Returns a reference to the member storing the set of included files.
    const std::unordered_set<std::string>& file_path_trace() const { return included_files_; }

private:
    // Used by GetInclude() to get the full filepath.
    const shaderc_util::FileFinder& file_finder_;
    // The full path and content of a source file.
    struct FileInfo
    {
        const std::string full_path;
        std::vector<char> contents;
    };

    // The set of full paths of included files.
    std::unordered_set<std::string> included_files_;
}; // class ShaderIncluder

shaderc_include_result* MakeErrorIncludeResult(const char* message)
{
    return new shaderc_include_result{"", 0, message, strlen(message)};
}

ShaderIncluder::~ShaderIncluder() = default;

shaderc_include_result* ShaderIncluder::GetInclude(const char* requested_source,
                                                   shaderc_include_type include_type,
                                                   const char* requesting_source, size_t)
{

    const std::string full_path =
        (include_type == shaderc_include_type_relative)
            ? file_finder_.FindRelativeReadableFilepath(requesting_source, requested_source)
            : file_finder_.FindReadableFilepath(requested_source);

    if (full_path.empty())
        return MakeErrorIncludeResult("Cannot find or open include file.");

    // In principle, several threads could be resolving includes at the same
    // time.  Protect the included_files.

    // Read the file and save its full path and contents into stable addresses.
    FileInfo* new_file_info = new FileInfo{full_path, {}};
    if (!shaderc_util::ReadFile(full_path, &(new_file_info->contents)))
    {
        return MakeErrorIncludeResult("Cannot read file");
    }

    included_files_.insert(full_path);

    return new shaderc_include_result{
        new_file_info->full_path.data(), new_file_info->full_path.length(),
        new_file_info->contents.data(), new_file_info->contents.size(), new_file_info};
}

void ShaderIncluder::ReleaseInclude(shaderc_include_result* include_result)
{
    FileInfo* info = static_cast<FileInfo*>(include_result->user_data);
    delete info;
    delete include_result;
}

} // anonymous namespace

namespace rad
{

GLSLCompiler::GLSLCompiler()
{
    m_compiler = std::make_unique<shaderc::Compiler>();
}

GLSLCompiler::~GLSLCompiler()
{
}

void GLSLCompiler::SetTargetVulkanVersion(shaderc_env_version version)
{
    m_targetVulkanVersion = version;
}

void GLSLCompiler::SetTargetSpirvVersion(shaderc_spirv_version version)
{
    m_targetSpirvVersion = version;
}

void GLSLCompiler::SetIncludeDirs(const std::vector<std::string>& includeDirs)
{
    m_includeDirs = includeDirs;
}

void GLSLCompiler::AddIncludeDir(std::string includeDir)
{
    m_includeDirs.push_back(std::move(includeDir));
}

Result<std::string, GLSLCompileError> GLSLCompiler::Preprocess(vk::ShaderStageFlagBits stage,
                                                               const std::string& fileName,
                                                               const std::string& source,
                                                               const std::string& entryPoint,
                                                               Span<GLSLMacro> macros)
{
    shaderc::CompileOptions options;

    options.SetSourceLanguage(shaderc_source_language_glsl);
    for (const auto& macro : macros)
    {
        options.AddMacroDefinition(macro.m_name, macro.m_value);
    }

    shaderc_util::FileFinder fileFinder;
    fileFinder.search_path() = m_includeDirs;
    std::unique_ptr<ShaderIncluder> includer(RAD_NEW ShaderIncluder(&fileFinder));
    options.SetIncluder(std::move(includer));
    options.SetTargetEnvironment(shaderc_target_env_vulkan, m_targetVulkanVersion);
    options.SetTargetSpirv(m_targetSpirvVersion);

    shaderc::PreprocessedSourceCompilationResult result =
        m_compiler->PreprocessGlsl(source, ToShaderKind(stage), fileName.c_str(), options);
    if (result.GetCompilationStatus() == shaderc_compilation_status_success)
    {
        return std::string{result.cbegin(), result.cend()};
    }
    else
    {
        return GLSLCompileError{result.GetErrorMessage()};
    }
}

Result<std::vector<uint32_t>, GLSLCompileError> GLSLCompiler::CompileToSpv(
    vk::ShaderStageFlagBits stage, const std::string& fileName, const std::string& source,
    const std::string& entryPoint, Span<GLSLMacro> macros, GLSLCompileOptLevel opt)
{
    shaderc::CompileOptions options;
    options.SetSourceLanguage(shaderc_source_language_glsl);
    for (const GLSLMacro& macro : macros)
    {
        options.AddMacroDefinition(macro.m_name, macro.m_value);
    }

    shaderc_util::FileFinder fileFinder;
    fileFinder.search_path() = m_includeDirs;
    std::unique_ptr<ShaderIncluder> includer(RAD_NEW ShaderIncluder(&fileFinder));
    options.SetIncluder(std::move(includer));
    options.SetTargetEnvironment(shaderc_target_env_vulkan, m_targetVulkanVersion);
    options.SetTargetSpirv(m_targetSpirvVersion);
    auto GetOptLevel = [](GLSLCompileOptLevel opt)
    {
        switch (opt)
        {
        case GLSLCompileOptLevel::Zero:
            return shaderc_optimization_level_zero;
        case GLSLCompileOptLevel::Size:
            return shaderc_optimization_level_size;
        case GLSLCompileOptLevel::Performance:
            return shaderc_optimization_level_performance;
        }
        return shaderc_optimization_level_zero;
    };
    options.SetOptimizationLevel(GetOptLevel(opt));

    shaderc::SpvCompilationResult result = m_compiler->CompileGlslToSpv(
        source, ToShaderKind(stage), fileName.c_str(), entryPoint.c_str(), options);

    if (result.GetCompilationStatus() == shaderc_compilation_status_success)
    {
        return std::vector<uint32_t>{result.cbegin(), result.cend()};
    }
    else
    {
        return GLSLCompileError{result.GetErrorMessage()};
    }
}

} // namespace rad
