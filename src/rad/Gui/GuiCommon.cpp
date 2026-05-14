#include <rad/Gui/GuiCommon.h>

namespace rad
{

GuiError::GuiError(std::string message) :
    m_message(std::move(message))
{
}

spdlog::logger* GetGuiLogger()
{
    static std::shared_ptr<spdlog::logger> logger = CreateLogger("Gui");
    return logger.get();
}

std::string SDL_GetEventDescription(const SDL_Event& event)
{
    // SDL3 docs: returns number of bytes needed for the full string, not counting the NUL byte.
    // `buf` may be NULL; the return value follows snprintf-style rules (never -1).
    // See: https://wiki.libsdl.org/SDL3/SDL_GetEventDescription
    const int byteCount = ::SDL_GetEventDescription(&event, nullptr, 0);
    if (byteCount <= 0)
    {
        return {};
    }

    std::string s;
    s.resize(static_cast<size_t>(byteCount) + 1); // +1 for NUL
    (void)::SDL_GetEventDescription(&event, s.data(), static_cast<int>(s.size()));
    s.resize(static_cast<size_t>(byteCount)); // trim NUL
    return s;
}

const char* SDL_GetEventName(Uint32 type)
{
    switch (type)
    {
    case SDL_EVENT_QUIT:
        return "SDL_EVENT_QUIT";
    case SDL_EVENT_TERMINATING:
        return "SDL_EVENT_TERMINATING";
    case SDL_EVENT_LOW_MEMORY:
        return "SDL_EVENT_LOW_MEMORY";
    case SDL_EVENT_WILL_ENTER_BACKGROUND:
        return "SDL_EVENT_WILL_ENTER_BACKGROUND";
    case SDL_EVENT_DID_ENTER_BACKGROUND:
        return "SDL_EVENT_DID_ENTER_BACKGROUND";
    case SDL_EVENT_WILL_ENTER_FOREGROUND:
        return "SDL_EVENT_WILL_ENTER_FOREGROUND";
    case SDL_EVENT_DID_ENTER_FOREGROUND:
        return "SDL_EVENT_DID_ENTER_FOREGROUND";
    case SDL_EVENT_LOCALE_CHANGED:
        return "SDL_EVENT_LOCALE_CHANGED";
    case SDL_EVENT_SYSTEM_THEME_CHANGED:
        return "SDL_EVENT_SYSTEM_THEME_CHANGED";

    case SDL_EVENT_DISPLAY_ORIENTATION:
        return "SDL_EVENT_DISPLAY_ORIENTATION";
    case SDL_EVENT_DISPLAY_ADDED:
        return "SDL_EVENT_DISPLAY_ADDED";
    case SDL_EVENT_DISPLAY_REMOVED:
        return "SDL_EVENT_DISPLAY_REMOVED";
    case SDL_EVENT_DISPLAY_MOVED:
        return "SDL_EVENT_DISPLAY_MOVED";
    case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
        return "SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED";

    case SDL_EVENT_WINDOW_SHOWN:
        return "SDL_EVENT_WINDOW_SHOWN";
    case SDL_EVENT_WINDOW_HIDDEN:
        return "SDL_EVENT_WINDOW_HIDDEN";
    case SDL_EVENT_WINDOW_EXPOSED:
        return "SDL_EVENT_WINDOW_EXPOSED";
    case SDL_EVENT_WINDOW_MOVED:
        return "SDL_EVENT_WINDOW_MOVED";
    case SDL_EVENT_WINDOW_RESIZED:
        return "SDL_EVENT_WINDOW_RESIZED";
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
        return "SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED";
    case SDL_EVENT_WINDOW_MINIMIZED:
        return "SDL_EVENT_WINDOW_MINIMIZED";
    case SDL_EVENT_WINDOW_MAXIMIZED:
        return "SDL_EVENT_WINDOW_MAXIMIZED";
    case SDL_EVENT_WINDOW_RESTORED:
        return "SDL_EVENT_WINDOW_RESTORED";
    case SDL_EVENT_WINDOW_MOUSE_ENTER:
        return "SDL_EVENT_WINDOW_MOUSE_ENTER";
    case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        return "SDL_EVENT_WINDOW_MOUSE_LEAVE";
    case SDL_EVENT_WINDOW_FOCUS_GAINED:
        return "SDL_EVENT_WINDOW_FOCUS_GAINED";
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        return "SDL_EVENT_WINDOW_FOCUS_LOST";
    case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
        return "SDL_EVENT_WINDOW_CLOSE_REQUESTED";
    case SDL_EVENT_WINDOW_HIT_TEST:
        return "SDL_EVENT_WINDOW_HIT_TEST";
    case SDL_EVENT_WINDOW_ICCPROF_CHANGED:
        return "SDL_EVENT_WINDOW_ICCPROF_CHANGED";
    case SDL_EVENT_WINDOW_DISPLAY_CHANGED:
        return "SDL_EVENT_WINDOW_DISPLAY_CHANGED";

    case SDL_EVENT_KEY_DOWN:
        return "SDL_EVENT_KEY_DOWN";
    case SDL_EVENT_KEY_UP:
        return "SDL_EVENT_KEY_UP";
    case SDL_EVENT_TEXT_EDITING:
        return "SDL_EVENT_TEXT_EDITING";
    case SDL_EVENT_TEXT_INPUT:
        return "SDL_EVENT_TEXT_INPUT";
    case SDL_EVENT_KEYMAP_CHANGED:
        return "SDL_EVENT_KEYMAP_CHANGED";
    case SDL_EVENT_KEYBOARD_ADDED:
        return "SDL_EVENT_KEYBOARD_ADDED";
    case SDL_EVENT_KEYBOARD_REMOVED:
        return "SDL_EVENT_KEYBOARD_REMOVED";

    case SDL_EVENT_MOUSE_MOTION:
        return "SDL_EVENT_MOUSE_MOTION";
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        return "SDL_EVENT_MOUSE_BUTTON_DOWN";
    case SDL_EVENT_MOUSE_BUTTON_UP:
        return "SDL_EVENT_MOUSE_BUTTON_UP";
    case SDL_EVENT_MOUSE_WHEEL:
        return "SDL_EVENT_MOUSE_WHEEL";
    case SDL_EVENT_MOUSE_ADDED:
        return "SDL_EVENT_MOUSE_ADDED";
    case SDL_EVENT_MOUSE_REMOVED:
        return "SDL_EVENT_MOUSE_REMOVED";

    case SDL_EVENT_JOYSTICK_AXIS_MOTION:
        return "SDL_EVENT_JOYSTICK_AXIS_MOTION";
    case SDL_EVENT_JOYSTICK_BALL_MOTION:
        return "SDL_EVENT_JOYSTICK_BALL_MOTION";
    case SDL_EVENT_JOYSTICK_HAT_MOTION:
        return "SDL_EVENT_JOYSTICK_HAT_MOTION";
    case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
        return "SDL_EVENT_JOYSTICK_BUTTON_DOWN";
    case SDL_EVENT_JOYSTICK_BUTTON_UP:
        return "SDL_EVENT_JOYSTICK_BUTTON_UP";
    case SDL_EVENT_JOYSTICK_ADDED:
        return "SDL_EVENT_JOYSTICK_ADDED";
    case SDL_EVENT_JOYSTICK_REMOVED:
        return "SDL_EVENT_JOYSTICK_REMOVED";
    case SDL_EVENT_JOYSTICK_BATTERY_UPDATED:
        return "SDL_EVENT_JOYSTICK_BATTERY_UPDATED";

    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        return "SDL_EVENT_GAMEPAD_AXIS_MOTION";
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        return "SDL_EVENT_GAMEPAD_BUTTON_DOWN";
    case SDL_EVENT_GAMEPAD_BUTTON_UP:
        return "SDL_EVENT_GAMEPAD_BUTTON_UP";
    case SDL_EVENT_GAMEPAD_ADDED:
        return "SDL_EVENT_GAMEPAD_ADDED";
    case SDL_EVENT_GAMEPAD_REMOVED:
        return "SDL_EVENT_GAMEPAD_REMOVED";
    case SDL_EVENT_GAMEPAD_REMAPPED:
        return "SDL_EVENT_GAMEPAD_REMAPPED";
    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
        return "SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN";
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION:
        return "SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION";
    case SDL_EVENT_GAMEPAD_TOUCHPAD_UP:
        return "SDL_EVENT_GAMEPAD_TOUCHPAD_UP";
    case SDL_EVENT_GAMEPAD_SENSOR_UPDATE:
        return "SDL_EVENT_GAMEPAD_SENSOR_UPDATE";
    case SDL_EVENT_GAMEPAD_UPDATE_COMPLETE:
        return "SDL_EVENT_GAMEPAD_UPDATE_COMPLETE";

    case SDL_EVENT_DROP_FILE:
        return "SDL_EVENT_DROP_FILE";
    case SDL_EVENT_DROP_TEXT:
        return "SDL_EVENT_DROP_TEXT";
    case SDL_EVENT_DROP_BEGIN:
        return "SDL_EVENT_DROP_BEGIN";
    case SDL_EVENT_DROP_COMPLETE:
        return "SDL_EVENT_DROP_COMPLETE";
    case SDL_EVENT_DROP_POSITION:
        return "SDL_EVENT_DROP_POSITION";

    case SDL_EVENT_AUDIO_DEVICE_ADDED:
        return "SDL_EVENT_AUDIO_DEVICE_ADDED";
    case SDL_EVENT_AUDIO_DEVICE_REMOVED:
        return "SDL_EVENT_AUDIO_DEVICE_REMOVED";
    case SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED:
        return "SDL_EVENT_AUDIO_DEVICE_FORMAT_CHANGED";

    case SDL_EVENT_RENDER_TARGETS_RESET:
        return "SDL_EVENT_RENDER_TARGETS_RESET";
    case SDL_EVENT_RENDER_DEVICE_RESET:
        return "SDL_EVENT_RENDER_DEVICE_RESET";
    default:
        break;
    }

    if (type >= SDL_EVENT_USER && type < SDL_EVENT_LAST)
    {
        return "SDL_EVENT_USER";
    }
    return "SDL_EVENT_UNKNOWN";
}

} // namespace rad
