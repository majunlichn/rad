#include <rad/Vulkan/SpvContext.h>

namespace rad
{

SpvContext::SpvContext()
{
    CreateContext();
}

SpvContext::~SpvContext()
{
    if (m_spvContext)
    {
        spvContextDestroy(m_spvContext);
        m_spvContext = nullptr;
    }
}

void SpvContext::CreateContext()
{
    if (m_spvContext)
    {
        spvContextDestroy(m_spvContext);
        m_spvContext = nullptr;
    }
    m_spvContext = spvContextCreate(m_targetEnv);
}

void SpvContext::SetTargetEnv(spv_target_env env)
{
    if (m_targetEnv == env)
    {
        return;
    }
    m_targetEnv = env;
    CreateContext();
}

Result<void, SpvError> SpvContext::Validate(const uint32_t* binary, size_t wordCount)
{
    SpvError err;
    spv_diagnostic diagnostic = nullptr;
    spv_const_binary_t spvBinary = {binary, wordCount};

    spv_result_t result = spvValidate(m_spvContext, &spvBinary, &diagnostic);
    if (result != SPV_SUCCESS)
    {
        if (diagnostic && diagnostic->error)
        {
            err.message = diagnostic->error;
        }
        else
        {
            err.message = "SPIR-V validation failed.";
        }
    }
    if (diagnostic)
    {
        spvDiagnosticDestroy(diagnostic);
    }
    if (result == SPV_SUCCESS)
    {
        return boost::outcome_v2::success();
    }
    return err;
}

Result<void, SpvError> SpvContext::Validate(Span<uint32_t> binary)
{
    return Validate(binary.data(), binary.size());
}

static int ToSpvDisassembleOptions(SpvContext::DisassembleOptions options)
{
    int out = 0;
    const uint32_t bits = static_cast<uint32_t>(options);
    if (bits & static_cast<uint32_t>(SpvContext::DisassembleOptions::FriendlyNames))
    {
        out |= SPV_BINARY_TO_TEXT_OPTION_FRIENDLY_NAMES;
    }
    if (bits & static_cast<uint32_t>(SpvContext::DisassembleOptions::Comments))
    {
        out |= SPV_BINARY_TO_TEXT_OPTION_COMMENT;
    }
    if (bits & static_cast<uint32_t>(SpvContext::DisassembleOptions::NestedIndent))
    {
        out |= SPV_BINARY_TO_TEXT_OPTION_NESTED_INDENT;
    }
    if (bits & static_cast<uint32_t>(SpvContext::DisassembleOptions::ReorderBlocks))
    {
        out |= SPV_BINARY_TO_TEXT_OPTION_REORDER_BLOCKS;
    }
    return out;
}

Result<std::string, SpvError> SpvContext::Disassemble(const uint32_t* binary, size_t wordCount,
                                                      DisassembleOptions options)
{
    std::string assembly;
    SpvError err;
    spv_text text = nullptr;
    spv_diagnostic diagnostic = nullptr;
    const int spvOptions = ToSpvDisassembleOptions(options);
    spv_result_t result =
        spvBinaryToText(m_spvContext, binary, wordCount, spvOptions, &text, &diagnostic);
    if (result == SPV_SUCCESS)
    {
        assembly = std::string(text->str, text->length);
    }
    else
    {
        if (diagnostic && diagnostic->error)
        {
            err.message = diagnostic->error;
        }
    }
    if (diagnostic)
    {
        spvDiagnosticDestroy(diagnostic);
    }
    if (text)
    {
        spvTextDestroy(text);
    }
    if (!assembly.empty())
    {
        return assembly;
    }
    else
    {
        return err;
    }
}

Result<std::string, SpvError> SpvContext::Disassemble(Span<uint32_t> binary)
{
    return Disassemble(binary.data(), binary.size(), DisassembleOptions::Default);
}

Result<std::vector<uint32_t>, SpvError> SpvContext::Assemble(std::string_view assembly)
{
    SpvError err;
    spv_binary binary = nullptr;
    spv_diagnostic diagnostic = nullptr;
    spv_result_t result =
        spvTextToBinary(m_spvContext, assembly.data(), assembly.size(), &binary, &diagnostic);

    std::vector<uint32_t> words;
    if (result == SPV_SUCCESS && binary)
    {
        words.assign(binary->code, binary->code + binary->wordCount);
    }
    else
    {
        if (diagnostic && diagnostic->error)
        {
            err.message = diagnostic->error;
        }
        else
        {
            err.message = "SPIR-V assembly failed.";
        }
    }

    if (diagnostic)
    {
        spvDiagnosticDestroy(diagnostic);
    }
    if (binary)
    {
        spvBinaryDestroy(binary);
    }

    if (!words.empty())
    {
        return words;
    }
    return err;
}

Result<std::string, SpvError> SpvContext::ValidateAndDisassemble(Span<uint32_t> binary,
                                                                 DisassembleOptions options)
{
    auto validateResult = Validate(binary);
    if (!validateResult)
    {
        return validateResult.error();
    }
    return Disassemble(binary.data(), binary.size(), options);
}

} // namespace rad
