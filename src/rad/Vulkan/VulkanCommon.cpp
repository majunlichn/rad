#include <rad/Vulkan/VulkanCommon.h>

namespace rad
{

spdlog::logger* GetVulkanLogger()
{
    static std::shared_ptr<spdlog::logger> logger = CreateLogger("Vulkan");
    return logger.get();
}

void ReportError(vk::Result result, const char* call, std::source_location sourceLoc)
{
    VK_LOG(err, "{} failed with {} (at {}:{} in {}).", call, vk::to_string(result),
           sourceLoc.file_name(), sourceLoc.line(), sourceLoc.function_name());
    throw vk::SystemError(vk::make_error_code(result), vk::to_string(result));
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugUtilsMessengerCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    vk::DebugUtilsMessageTypeFlagsEXT messageTypes,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    switch (messageSeverity)
    {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
        VK_LOG(debug, "[{}] {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
        VK_LOG(info, "[{}] {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        VK_LOG(warn, "[{}] {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        VK_LOG(err, "[{}] {}", pCallbackData->pMessageIdName, pCallbackData->pMessage);
#if defined(_DEBUG)
#if defined(RAD_COMPILER_MSVC)
        __debugbreak();
#endif // MSVC
#endif // _DEBUG
        break;
    }

    return VK_FALSE;
}

} // namespace rad
