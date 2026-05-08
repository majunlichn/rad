#include <rad/Multimedia/MultimediaCommon.h>

namespace rad
{

const char* AudioFormatToString(SDL_AudioFormat format)
{
    switch (format)
    {
    case SDL_AUDIO_UNKNOWN:
        return "Unknown";
    case SDL_AUDIO_U8:
        return "U8";
    case SDL_AUDIO_S8:
        return "S8";
    case SDL_AUDIO_S16LE:
        return "S16LE";
    case SDL_AUDIO_S16BE:
        return "S16BE";
    case SDL_AUDIO_S32LE:
        return "S32LE";
    case SDL_AUDIO_S32BE:
        return "S32BE";
    case SDL_AUDIO_F32LE:
        return "F32LE";
    case SDL_AUDIO_F32BE:
        return "F32BE";
    default:
        return "Unknown";
    }
}

spdlog::logger* GetMultimediaLogger()
{
    static std::shared_ptr<spdlog::logger> logger = CreateLogger("Multimedia");
    return logger.get();
}

} // namespace rad
