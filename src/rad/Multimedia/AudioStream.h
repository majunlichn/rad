#pragma once

#include <rad/Multimedia/MultimediaCommon.h>

#include <rad/Common/String.h>

#include <SDL3/SDL_audio.h>

#include <vector>

namespace rad
{

class AudioDevice;
class AudioStream;

// https://wiki.libsdl.org/SDL3/SDL_LoadWAV_IO
bool LoadWav(SDL_IOStream* src, bool closeio, SDL_AudioSpec* spec, SDL_Buffer* buffer,
             Uint32* sizeInBytes);
// https://wiki.libsdl.org/SDL3/SDL_LoadWAV
bool LoadWavFromFile(cstring_view path, SDL_AudioSpec* spec, SDL_Buffer* buffer,
                     Uint32* sizeInBytes);
// https://wiki.libsdl.org/SDL3/SDL_MixAudio
bool MixAudio(Uint8* dst, const Uint8* src, SDL_AudioFormat format, Uint32 sizeInBytes,
              float volume);
// https://wiki.libsdl.org/SDL3/SDL_ConvertAudioSamples
bool ConvertAudioSamples(const SDL_AudioSpec* srcSpec, const Uint8* srcData, int srcSizeInBytes,
                         const SDL_AudioSpec* dstSpec, SDL_Buffer* dstData, int* dstSizeInBytes);
// https://wiki.libsdl.org/SDL3/SDL_GetAudioFormatName
const char* GetAudioFormatName(SDL_AudioFormat format);
// https://wiki.libsdl.org/SDL3/SDL_GetSilenceValueForFormat
int GetSilenceValueForFormat(SDL_AudioFormat format);

// SDL_AudioStream wrapper (conversion, device binding, I/O).
// https://wiki.libsdl.org/SDL3/CategoryAudio
class AudioStream : public RefCounted<AudioStream>
{
public:
    // https://wiki.libsdl.org/SDL3/SDL_CreateAudioStream
    static Ref<AudioStream> Create(const SDL_AudioSpec* srcSpec, const SDL_AudioSpec* dstSpec);

    // SDL_OpenAudioDeviceStream; call ResumeDevice() if the stream device starts paused.
    // https://wiki.libsdl.org/SDL3/SDL_OpenAudioDeviceStream
    static Ref<AudioStream> OpenDevice(SDL_AudioDeviceID deviceId, const SDL_AudioSpec* spec,
                                       SDL_AudioStreamCallback callback = nullptr,
                                       void* userData = nullptr);
    // Delegates to OpenDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, ...).
    static Ref<AudioStream> OpenDefaultPlayback(const SDL_AudioSpec* spec,
                                                SDL_AudioStreamCallback callback = nullptr,
                                                void* userData = nullptr);
    // Delegates to OpenDevice(SDL_AUDIO_DEVICE_DEFAULT_RECORDING, ...).
    static Ref<AudioStream> OpenDefaultRecording(const SDL_AudioSpec* spec,
                                                 SDL_AudioStreamCallback callback = nullptr,
                                                 void* userData = nullptr);
    // Calls Destroy().
    // https://wiki.libsdl.org/SDL3/SDL_DestroyAudioStream
    ~AudioStream();

    // https://wiki.libsdl.org/SDL3/SDL_DestroyAudioStream
    void Destroy();

    // Underlying SDL_AudioStream* for passing to other SDL audio APIs.
    SDL_AudioStream* GetHandle() const { return m_handle; }
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamDevice
    SDL_AudioDeviceID GetDeviceId() const;
    // Cached from SDL_GetAudioStreamProperties in the constructor.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamProperties
    SDL_PropertiesID GetPropertiesId() const { return m_propertiesId; }
    Ref<AudioDevice> GetDevice() const { return m_device; }

    // Binds this stream to an AudioDevice opened with SDL_OpenAudioDevice (uses its logical device ID).
    // https://wiki.libsdl.org/SDL3/SDL_BindAudioStream
    bool BindToDevice(Ref<AudioDevice> device);
    // https://wiki.libsdl.org/SDL3/SDL_UnbindAudioStream
    void UnbindFromDevice();

    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamFormat
    bool GetFormat(SDL_AudioSpec* srcSpec, SDL_AudioSpec* dstSpec);
    // https://wiki.libsdl.org/SDL3/SDL_SetAudioStreamFormat
    bool SetFormat(const SDL_AudioSpec* srcSpec, const SDL_AudioSpec* dstSpec);
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamFrequencyRatio
    float GetFrequencyRatio();
    // https://wiki.libsdl.org/SDL3/SDL_SetAudioStreamFrequencyRatio
    bool SetFrequencyRatio(float ratio);
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamGain
    float GetGain() const;
    // https://wiki.libsdl.org/SDL3/SDL_SetAudioStreamGain
    bool SetGain(float gain) const;
    // Copies the map; frees SDL's buffer with SDL_free.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamInputChannelMap
    // https://wiki.libsdl.org/SDL3/SDL_free
    std::vector<int> GetInputChannelMap() const;
    // Copies the map; frees SDL's buffer with SDL_free.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamOutputChannelMap
    // https://wiki.libsdl.org/SDL3/SDL_free
    std::vector<int> GetOutputChannelMap() const;
    // https://wiki.libsdl.org/SDL3/SDL_SetAudioStreamInputChannelMap
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamFormat
    bool SetInputChannelMap(Span<int> map);
    // https://wiki.libsdl.org/SDL3/SDL_SetAudioStreamOutputChannelMap
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamFormat
    bool SetOutputChannelMap(Span<int> map);
    // https://wiki.libsdl.org/SDL3/SDL_PutAudioStreamData
    bool PutData(const void* data, int sizeInBytes);
    // https://wiki.libsdl.org/SDL3/SDL_PutAudioStreamDataNoCopy
    bool PutDataNoCopy(const void* data, int sizeInBytes,
                       SDL_AudioStreamDataCompleteCallback callback = nullptr,
                       void* userData = nullptr);
    // https://wiki.libsdl.org/SDL3/SDL_PutAudioStreamPlanarData
    bool PutPlanarData(const void* const* data, int numChannels, int numSamples);
    // Returns bytes read, or -1 on failure.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamData
    int GetData(void* data, int sizeInBytes);
    // Converted or resampled bytes ready to read.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamAvailable
    int GetDataSizeAvailable();
    // Raw input bytes currently queued in the stream.
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamQueued
    int GetDataSizeQueued();
    // https://wiki.libsdl.org/SDL3/SDL_FlushAudioStream
    bool Flush();
    // https://wiki.libsdl.org/SDL3/SDL_ClearAudioStream
    bool Clear();
    // https://wiki.libsdl.org/SDL3/SDL_PauseAudioStreamDevice
    bool PauseDevice();
    // https://wiki.libsdl.org/SDL3/SDL_ResumeAudioStreamDevice
    bool ResumeDevice();
    // https://wiki.libsdl.org/SDL3/SDL_AudioStreamDevicePaused
    bool IsDevicePaused();
    // https://wiki.libsdl.org/SDL3/SDL_LockAudioStream
    bool Lock();
    // https://wiki.libsdl.org/SDL3/SDL_UnlockAudioStream
    bool Unlock();

    // https://wiki.libsdl.org/SDL3/SDL_SetAudioStreamGetCallback
    bool SetGetCallback(SDL_AudioStreamCallback callback, void* userData);
    // https://wiki.libsdl.org/SDL3/SDL_SetAudioStreamPutCallback
    bool SetPutCallback(SDL_AudioStreamCallback callback, void* userData);

private:
    // https://wiki.libsdl.org/SDL3/SDL_GetAudioStreamProperties
    AudioStream(SDL_AudioStream* handle);
    SDL_AudioStream* m_handle;
    SDL_PropertiesID m_propertiesId = 0;
    // Keeps the bound device alive (nullptr if unbound or stream owns internal device).
    Ref<AudioDevice> m_device;

}; // class AudioStream

} // namespace rad
