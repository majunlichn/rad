#include <rad/Gui/GuiCommon.h>

namespace rad
{

spdlog::logger* GetGuiLogger()
{
    static std::shared_ptr<spdlog::logger> logger = CreateLogger("GUI");
    return logger.get();
}

} // namespace rad
