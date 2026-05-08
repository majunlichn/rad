#include <rad/Common/MemoryDebug.h>

#include <rad/Multimedia/AudioDevice.h>

#include <cstring>

namespace rad
{

AudioSystem::AudioSystem()
{
}

AudioSystem::~AudioSystem()
{
}

std::vector<const char*> AudioSystem::GetAudioDrivers()
{
    std::vector<const char*> drivers;
    int driverCount = SDL_GetNumAudioDrivers();
    if (driverCount > 0)
    {
        drivers.resize(driverCount);
        for (int i = 0; i < driverCount; ++i)
        {
            drivers[i] = SDL_GetAudioDriver(i);
        }
    }
    return drivers;
}

const char* AudioSystem::GetCurrentAudioDriver()
{
    return SDL_GetCurrentAudioDriver();
}

AudioSystem& AudioSystem::GetInstance()
{
    static AudioSystem instance;
    return instance;
}

bool AudioSystem::Init()
{
    if (m_initialized)
    {
        return true;
    }
    m_initialized = SDL_InitSubSystem(SDL_INIT_AUDIO);
    if (!m_initialized)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_InitSubSystem(SDL_INIT_AUDIO) failed: {}", SDL_GetError());
    }
    return m_initialized;
}

void AudioSystem::Shutdown()
{
    if (m_initialized)
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        m_initialized = false;
    }
}

std::vector<Ref<AudioDevice>> AudioSystem::GetAudioPlaybackDevices()
{
    std::vector<Ref<AudioDevice>> devices;
    int count = 0;
    SDL_ClearError();
    SDL_AudioDeviceID* ids = SDL_GetAudioPlaybackDevices(&count);
    if (ids)
    {
        if (count > 0)
        {
            devices.resize(count);
            for (int i = 0; i < count; ++i)
            {
                devices[i] = RAD_NEW AudioDevice(ids[i]);
            }
        }
        SDL_free(ids);
        ids = nullptr;
    }
    else if (!ids)
    {
        const char* err = SDL_GetError();
        if (err && *err)
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioPlaybackDevices failed: {}", err);
        }
    }
    return devices;
}

std::vector<Ref<AudioDevice>> AudioSystem::GetAudioRecordingDevices()
{
    std::vector<Ref<AudioDevice>> devices;
    int count = 0;
    SDL_ClearError();
    SDL_AudioDeviceID* ids = SDL_GetAudioRecordingDevices(&count);
    if (ids)
    {
        if (count > 0)
        {
            devices.resize(count);
            for (int i = 0; i < count; ++i)
            {
                devices[i] = RAD_NEW AudioDevice(ids[i]);
            }
        }
        SDL_free(ids);
        ids = nullptr;
    }
    else if (!ids)
    {
        const char* err = SDL_GetError();
        if (err && *err)
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioRecordingDevices failed: {}", err);
        }
    }
    return devices;
}

AudioDevice::AudioDevice(SDL_AudioDeviceID instanceId) :
    m_instanceId(instanceId)
{
    const char* name = SDL_GetAudioDeviceName(m_instanceId);
    if (name)
    {
        m_name = name;
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioDeviceName failed: {}", SDL_GetError());
    }

    if (!SDL_GetAudioDeviceFormat(m_instanceId, &m_spec, &m_sampleFrames))
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioDeviceFormat failed: {}", SDL_GetError());
    }
}

AudioDevice::~AudioDevice()
{
    Close();
}

bool AudioDevice::Open(const SDL_AudioSpec* spec)
{
    m_id = SDL_OpenAudioDevice(m_instanceId, spec);
    if (m_id == 0)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_OpenAudioDevice failed: {}", SDL_GetError());
        return false;
    }
    // For opened devices, query the current format using the logical device ID.
    if (!SDL_GetAudioDeviceFormat(m_id, &m_spec, &m_sampleFrames))
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioDeviceFormat failed: {}", SDL_GetError());
    }
    return true;
}

void AudioDevice::Close()
{
    if (m_id != 0)
    {
        SDL_CloseAudioDevice(m_id);
        m_id = 0;
        // After closing, fall back to the device's preferred format.
        if (!SDL_GetAudioDeviceFormat(m_instanceId, &m_spec, &m_sampleFrames))
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioDeviceFormat failed: {}", SDL_GetError());
            m_spec = {};
            m_sampleFrames = 0;
        }
    }
}

std::vector<int> AudioDevice::GetChannelMap() const
{
    std::vector<int> buffer;
    int count = 0;
    int* channelMap = SDL_GetAudioDeviceChannelMap(m_id, &count);
    if (channelMap)
    {
        if (count > 0)
        {
            buffer.resize(count);
            std::memcpy(buffer.data(), channelMap, count * sizeof(int));
        }
        SDL_free(channelMap);
        channelMap = nullptr;
    }
    return buffer;
}

bool AudioDevice::IsPlayback() const
{
    assert(IsOpened());
    return SDL_IsAudioDevicePlayback(m_id);
}

bool AudioDevice::Pause()
{
    assert(IsOpened());
    const bool result = SDL_PauseAudioDevice(m_id);
    if (result)
    {
        RAD_LOG_MULTIMEDIA(info, "AudioDevice {} (id#{}) is paused.", m_name, m_id);
        return true;
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_PauseAudioDevice failed: {}", SDL_GetError());
        return false;
    }
}

bool AudioDevice::Resume()
{
    assert(IsOpened());
    const bool result = SDL_ResumeAudioDevice(m_id);
    if (result)
    {
        RAD_LOG_MULTIMEDIA(info, "AudioDevice {} (id#{}) is resumed.", m_name, m_id);
        return true;
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_ResumeAudioDevice failed: {}", SDL_GetError());
        return false;
    }
}

bool AudioDevice::IsPaused() const
{
    assert(IsOpened());
    return (SDL_AudioDevicePaused(m_id) == true);
}

float AudioDevice::GetGain() const
{
    assert(IsOpened());
    return SDL_GetAudioDeviceGain(m_id);
}

bool AudioDevice::SetGain(float gain) const
{
    assert(IsOpened());
    const bool result = SDL_SetAudioDeviceGain(m_id, gain);
    if (result)
    {
        RAD_LOG_MULTIMEDIA(info, "AudioDevice {} (id#{}): gain is set to {}", m_name, m_id, gain);
        return true;
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioDeviceGain failed: {}", SDL_GetError());
        return false;
    }
}

bool AudioDevice::BindStreams(SDL_AudioStream* const* streams, int streamCount)
{
    assert(IsOpened());
    if (!streams || (streamCount <= 0))
    {
        return true;
    }

    const bool result = SDL_BindAudioStreams(m_id, streams, streamCount);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_BindAudioStreams failed: {}", SDL_GetError());
    }
    return result;
}

void AudioDevice::UnbindStreams(SDL_AudioStream* const* streams, int streamCount)
{
    // SDL3 unbind is per-stream and does not take a device id.
    for (int i = 0; i < streamCount; ++i)
    {
        if (streams[i])
        {
            SDL_UnbindAudioStream(streams[i]);
        }
    }
}

bool AudioDevice::BindStream(SDL_AudioStream* stream)
{
    assert(IsOpened());
    if (!stream)
    {
        return false;
    }

    const bool result = SDL_BindAudioStream(m_id, stream);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_BindAudioStream failed: {}", SDL_GetError());
    }
    return result;
}

void AudioDevice::UnbindAudioStream(SDL_AudioStream* stream)
{
    assert(IsOpened());
    SDL_UnbindAudioStream(stream);
}

bool AudioDevice::SetPostmixCallback(SDL_AudioPostmixCallback callback, void* userData)
{
    assert(IsOpened());
    const bool result = SDL_SetAudioPostmixCallback(m_id, callback, userData);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioPostmixCallback failed: {}", SDL_GetError());
    }
    return result;
}

} // namespace rad
