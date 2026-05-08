#pragma once

#include <rad/Common/Platform.h>
#include <rad/Common/Integer.h>
#include <rad/Common/Float.h>
#include <rad/Common/Flags.h>
#include <rad/Common/Memory.h>
#include <rad/Common/RefCounted.h>
#include <rad/Container/SmallVector.h>
#include <rad/Container/Span.h>
#include <rad/IO/Logging.h>
#include <rad/System/Time.h>

#include <SDL3/SDL.h>

#include <chrono>
#include <format>
#include <source_location>
#include <stdexcept>

namespace rad
{

// SDL-allocated buffer that must be freed with SDL_free().
class SDL_Buffer
{
public:
    explicit SDL_Buffer(Uint8* buffer) :
        m_data(buffer)
    {
    }

    SDL_Buffer(const SDL_Buffer&) = delete;
    SDL_Buffer& operator=(const SDL_Buffer&) = delete;

    SDL_Buffer(SDL_Buffer&& other) noexcept :
        m_data(other.m_data)
    {
        other.m_data = nullptr;
    }

    SDL_Buffer& operator=(SDL_Buffer&& other) noexcept
    {
        if (this != &other)
        {
            if (m_data)
            {
                SDL_free(m_data);
            }
            m_data = other.m_data;
            other.m_data = nullptr;
        }
        return *this;
    }

    ~SDL_Buffer()
    {
        if (m_data)
        {
            SDL_free(m_data);
            m_data = nullptr;
        }
    }

    Uint8* m_data = nullptr;

}; // class SDL_Buffer

const char* AudioFormatToString(SDL_AudioFormat format);

spdlog::logger* GetMultimediaLogger();

// LogLevel: trace, debug, info, warn, err, critical
#define RAD_LOG_MULTIMEDIA(LogLevel, ...)                                                          \
    SPDLOG_LOGGER_CALL(::rad::GetMultimediaLogger(), spdlog::level::LogLevel, __VA_ARGS__)

class MultimediaException : public std::runtime_error
{
public:
    MultimediaException(const std::string& message) :
        std::runtime_error(message)
    {
    }
}; // class MultimediaException

} // namespace rad
