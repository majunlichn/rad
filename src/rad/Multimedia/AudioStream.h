#pragma once

#include <rad/Multimedia/MultimediaCommon.h>

#include <rad/Common/String.h>

#include <SDL3/SDL_audio.h>

#include <vector>

// https://wiki.libsdl.org/SDL3/CategoryAudio

namespace rad
{

class AudioDevice;
class AudioStream;

bool LoadWav(SDL_IOStream* src, bool closeio, SDL_AudioSpec* spec, SDL_Buffer* buffer,
             Uint32* sizeInBytes);
bool LoadWavFromFile(cstring_view path, SDL_AudioSpec* spec, SDL_Buffer* buffer,
                     Uint32* sizeInBytes);
bool MixAudio(Uint8* dst, const Uint8* src, SDL_AudioFormat format, Uint32 sizeInBytes,
              float volume);
bool ConvertAudioSamples(const SDL_AudioSpec* srcSpec, const Uint8* srcData, int srcSizeInBytes,
                         const SDL_AudioSpec* dstSpec, SDL_Buffer* dstData, int* dstSizeInBytes);
const char* GetAudioFormatName(SDL_AudioFormat format);
int GetSilenceValueForFormat(SDL_AudioFormat format);

class AudioStream : public RefCounted<AudioStream>
{
public:
    // Create a pure conversion/resample stream (not bound to any device yet).
    static Ref<AudioStream> Create(const SDL_AudioSpec* srcSpec, const SDL_AudioSpec* dstSpec);

    // Stream-first SDL3 style: open a device, create a stream, and bind them in one call.
    // The device starts paused; call ResumeDevice() to start audio flow.
    static Ref<AudioStream> OpenDevice(SDL_AudioDeviceID deviceId, const SDL_AudioSpec* spec,
                                       SDL_AudioStreamCallback callback = nullptr,
                                       void* userData = nullptr);
    static Ref<AudioStream> OpenDefaultPlayback(const SDL_AudioSpec* spec,
                                                SDL_AudioStreamCallback callback = nullptr,
                                                void* userData = nullptr);
    static Ref<AudioStream> OpenDefaultRecording(const SDL_AudioSpec* spec,
                                                 SDL_AudioStreamCallback callback = nullptr,
                                                 void* userData = nullptr);
    ~AudioStream();

    void Destroy();

    SDL_AudioStream* GetHandle() const { return m_handle; }
    SDL_AudioDeviceID GetDeviceId() const;
    SDL_PropertiesID GetPropertiesId() const { return m_propertiesId; }
    Ref<AudioDevice> GetDevice() const { return m_device; }

    // Advanced path: bind/unbind a converter stream to an AudioDevice opened via SDL_OpenAudioDevice().
    bool BindToDevice(Ref<AudioDevice> device);
    void UnbindFromDevice();

    bool GetFormat(SDL_AudioSpec* srcSpec, SDL_AudioSpec* dstSpec);
    bool SetFormat(const SDL_AudioSpec* srcSpec, const SDL_AudioSpec* dstSpec);
    float GetFrequencyRatio();
    bool SetFrequencyRatio(float ratio);
    float GetGain() const;
    bool SetGain(float gain) const;
    std::vector<int> GetInputChannelMap() const;
    std::vector<int> GetOutputChannelMap() const;
    bool SetInputChannelMap(Span<int> map);
    bool SetOutputChannelMap(Span<int> map);
    bool PutData(const void* data, int sizeInBytes);
    bool PutDataNoCopy(const void* data, int sizeInBytes,
                       SDL_AudioStreamDataCompleteCallback callback = nullptr,
                       void* userData = nullptr);
    bool PutPlanarData(const void* const* data, int numChannels, int numSamples);
    // Returns the number of bytes read from the stream or -1 on failure.
    int GetData(void* data, int sizeInBytes);
    // Get the number of converted/resampled bytes available.
    int GetDataSizeAvailable();
    // Get the number of input (unconverted) bytes currently queued in the stream.
    int GetDataSizeQueued();
    bool Flush();
    bool Clear();
    bool PauseDevice();
    bool ResumeDevice();
    bool IsDevicePaused();
    bool Lock();
    bool Unlock();

    bool SetGetCallback(SDL_AudioStreamCallback callback, void* userData);
    bool SetPutCallback(SDL_AudioStreamCallback callback, void* userData);

private:
    AudioStream(SDL_AudioStream* handle);
    SDL_AudioStream* m_handle;
    SDL_PropertiesID m_propertiesId = 0;
    // Keeps the bound device alive (nullptr if unbound or stream owns internal device).
    Ref<AudioDevice> m_device;

}; // class AudioStream

} // namespace rad
