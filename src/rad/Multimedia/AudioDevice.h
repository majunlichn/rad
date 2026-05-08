#pragma once

#include <rad/Multimedia/MultimediaCommon.h>

#include <SDL3/SDL_audio.h>

#include <vector>

// https://wiki.libsdl.org/SDL3/CategoryAudio

namespace rad
{

class AudioDevice;

class AudioSystem
{
public:
    static AudioSystem& GetInstance();

    bool Init();
    void Shutdown();

    bool IsInitialized() const { return m_initialized; }

    std::vector<const char*> GetAudioDrivers();
    const char* GetCurrentAudioDriver();

    std::vector<Ref<AudioDevice>> GetAudioPlaybackDevices();
    std::vector<Ref<AudioDevice>> GetAudioRecordingDevices();

private:
    AudioSystem();
    ~AudioSystem();
    bool m_initialized = false;

}; // class AudioSystem

class AudioStream;

class AudioDevice : public RefCounted<AudioDevice>
{
public:
    AudioDevice(SDL_AudioDeviceID deviceId);
    ~AudioDevice();

    bool Open(const SDL_AudioSpec* spec = nullptr);
    void Close();

    // Logical device id returned by SDL_OpenAudioDevice.
    SDL_AudioDeviceID GetId() const { return m_id; }
    bool IsOpened() const { return m_id != 0; }

    const std::string& GetName() const { return m_name; }
    const SDL_AudioSpec& GetSpec() const { return m_spec; }
    int GetSampleFrames() const { return m_sampleFrames; }
    std::vector<int> GetChannelMap() const;

    bool IsPlayback() const;
    bool Pause();
    bool Resume();
    bool IsPaused() const;
    // The gain of a device is its volume; a larger gain means a louder output,
    // with a gain of zero being silence.
    // Audio devices default to a gain of 1.0f (no change in output).
    float GetGain() const;
    bool SetGain(float gain) const;

    bool BindStreams(SDL_AudioStream* const* streams, int streamCount);
    void UnbindStreams(SDL_AudioStream* const* streams, int streamCount);
    bool BindStream(SDL_AudioStream* stream);
    void UnbindAudioStream(SDL_AudioStream* stream);

    bool SetPostmixCallback(SDL_AudioPostmixCallback callback, void* userData);

private:
    SDL_AudioDeviceID m_instanceId = 0;
    SDL_AudioDeviceID m_id = 0; // logical id
    std::string m_name;
    SDL_AudioSpec m_spec = {};
    int m_sampleFrames = 0;

}; // class AudioDevice

} // namespace rad
