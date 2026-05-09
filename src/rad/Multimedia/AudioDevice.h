#pragma once

#include <rad/Multimedia/MultimediaCommon.h>

#include <SDL3/SDL_audio.h>

#include <vector>

namespace rad
{

class AudioDevice;

// Global SDL audio subsystem (init / drivers / device enumeration).
// https://wiki.libsdl.org/SDL3/CategoryAudio
class AudioSystem
{
public:
    // Returns the process-wide AudioSystem instance.
    static AudioSystem& GetInstance();

    // https://wiki.libsdl.org/SDL3/SDL_InitSubSystem
    bool Init();
    // https://wiki.libsdl.org/SDL3/SDL_QuitSubSystem
    void Shutdown();

    // True after a successful Init() (SDL_INIT_AUDIO).
    bool IsInitialized() const { return m_initialized; }

    // https://wiki.libsdl.org/SDL3/SDL_GetNumAudioDrivers
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioDriver
    std::vector<const char*> GetAudioDrivers();
    // https://wiki.libsdl.org/SDL3/SDL_GetCurrentAudioDriver
    const char* GetCurrentAudioDriver();

    // Enumerates playback devices; frees the returned ID array with SDL_free.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioPlaybackDevices
    // https://wiki.libsdl.org/SDL3/SDL_free
    std::vector<Ref<AudioDevice>> GetAudioPlaybackDevices();
    // Enumerates recording devices; frees the returned ID array with SDL_free.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioRecordingDevices
    // https://wiki.libsdl.org/SDL3/SDL_free
    std::vector<Ref<AudioDevice>> GetAudioRecordingDevices();

private:
    AudioSystem();
    ~AudioSystem();
    bool m_initialized = false;

}; // class AudioSystem

class AudioStream;

// SDL audio device: enumeration instance ID before Open; logical device ID after Open.
// https://wiki.libsdl.org/SDL3/CategoryAudio
class AudioDevice : public RefCounted<AudioDevice>
{
public:
    // SDL_GetAudioDeviceName and SDL_GetAudioDeviceFormat for the physical instance id.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioDeviceName
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioDeviceFormat
    AudioDevice(SDL_AudioDeviceID deviceId);
    // Calls Close().
    // https://wiki.libsdl.org/SDL3/SDL_CloseAudioDevice
    ~AudioDevice();

    // SDL_OpenAudioDevice, then SDL_GetAudioDeviceFormat on the logical id.
    // https://wiki.libsdl.org/SDL3/SDL_OpenAudioDevice
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioDeviceFormat
    bool Open(const SDL_AudioSpec* spec = nullptr);
    // SDL_CloseAudioDevice, then SDL_GetAudioDeviceFormat on the physical instance ID to refresh the cached spec.
    // https://wiki.libsdl.org/SDL3/SDL_CloseAudioDevice
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioDeviceFormat
    void Close();

    // Logical device id from SDL_OpenAudioDevice (0 before Open).
    SDL_AudioDeviceID GetId() const { return m_id; }
    bool IsOpened() const { return m_id != 0; }

    const std::string& GetName() const { return m_name; }
    const SDL_AudioSpec& GetSpec() const { return m_spec; }
    int GetSampleFrames() const { return m_sampleFrames; }
    // Copies the channel map and frees SDL's buffer with SDL_free.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioDeviceChannelMap
    // https://wiki.libsdl.org/SDL3/SDL_free
    std::vector<int> GetChannelMap() const;

    // https://wiki.libsdl.org/SDL3/SDL_IsAudioDevicePlayback
    bool IsPlayback() const;
    // https://wiki.libsdl.org/SDL3/SDL_PauseAudioDevice
    bool Pause();
    // https://wiki.libsdl.org/SDL3/SDL_ResumeAudioDevice
    bool Resume();
    // https://wiki.libsdl.org/SDL3/SDL_AudioDevicePaused
    bool IsPaused() const;
    // The gain of a device is its volume; a larger gain means a louder output,
    // with a gain of zero being silence.
    // Audio devices default to a gain of 1.0f (no change in output).
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioDeviceGain
    float GetGain() const;
    // https://wiki.libsdl.org/SDL3/SDL_SetAudioDeviceGain
    bool SetGain(float gain) const;

    // https://wiki.libsdl.org/SDL3/SDL_BindAudioStreams
    bool BindStreams(SDL_AudioStream* const* streams, int streamCount);
    // Calls SDL_UnbindAudioStream for each non-null stream.
    // https://wiki.libsdl.org/SDL3/SDL_UnbindAudioStream
    void UnbindStreams(SDL_AudioStream* const* streams, int streamCount);
    // https://wiki.libsdl.org/SDL3/SDL_BindAudioStream
    bool BindStream(SDL_AudioStream* stream);
    // https://wiki.libsdl.org/SDL3/SDL_UnbindAudioStream
    void UnbindAudioStream(SDL_AudioStream* stream);

    // https://wiki.libsdl.org/SDL3/SDL_SetAudioPostmixCallback
    bool SetPostmixCallback(SDL_AudioPostmixCallback callback, void* userData);

private:
    SDL_AudioDeviceID m_instanceId = 0;
    SDL_AudioDeviceID m_id = 0; // logical id
    std::string m_name;
    SDL_AudioSpec m_spec = {};
    int m_sampleFrames = 0;

}; // class AudioDevice

} // namespace rad
