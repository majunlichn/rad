#include <rad/Gui/Gui.test.h>

#include <gtest/gtest.h>

using namespace rad;

TEST(GuiApplication, CheckSubsystem)
{
    SDL_InitFlags initFlags = g_env->GetApp()->GetInitFlags();
    EXPECT_TRUE(HasBits(initFlags, SDL_INIT_EVENTS));
    std::string subsystemNames;
    if (HasBits(initFlags, SDL_INIT_AUDIO))
    {
        subsystemNames += "Audio;";
    }
    if (HasBits(initFlags, SDL_INIT_VIDEO))
    {
        subsystemNames += "Video;";
    }
    if (HasBits(initFlags, SDL_INIT_JOYSTICK))
    {
        subsystemNames += "Joystick;";
    }
    if (HasBits(initFlags, SDL_INIT_HAPTIC))
    {
        subsystemNames += "Haptic;";
    }
    if (HasBits(initFlags, SDL_INIT_GAMEPAD))
    {
        subsystemNames += "Gamepad;";
    }
    if (HasBits(initFlags, SDL_INIT_EVENTS))
    {
        subsystemNames += "Events;";
    }
    if (HasBits(initFlags, SDL_INIT_SENSOR))
    {
        subsystemNames += "Sensor;";
    }
    if (HasBits(initFlags, SDL_INIT_CAMERA))
    {
        subsystemNames += "Camera;";
    }
    RAD_LOG_GUI(info, "SDL subsystems initialized: {}", subsystemNames);
}
