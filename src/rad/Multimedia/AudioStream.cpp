#include <rad/Common/MemoryDebug.h>

#include <rad/Multimedia/AudioStream.h>
#include <rad/Multimedia/AudioDevice.h>

namespace rad
{

bool LoadWav(SDL_IOStream* src, bool closeio, SDL_AudioSpec* spec, SDL_Buffer* buffer,
             Uint32* sizeInBytes)
{
    if (!src || !spec || !buffer || !sizeInBytes)
    {
        RAD_LOG_MULTIMEDIA(err, "LoadWav invalid argument(s).");
        return false;
    }
    const bool result = SDL_LoadWAV_IO(src, closeio, spec, &buffer->m_data, sizeInBytes);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_LoadWAV_IO failed: {}", SDL_GetError());
    }
    return result;
}

bool LoadWavFromFile(cstring_view path, SDL_AudioSpec* spec, SDL_Buffer* buffer,
                     Uint32* sizeInBytes)
{
    if (!spec || !buffer || !sizeInBytes)
    {
        RAD_LOG_MULTIMEDIA(err, "LoadWavFromFile invalid argument(s).");
        return false;
    }
    const bool result = SDL_LoadWAV(path.c_str(), spec, &buffer->m_data, sizeInBytes);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_LoadWAV failed: {}", SDL_GetError());
    }
    return result;
}

bool MixAudio(Uint8* dst, const Uint8* src, SDL_AudioFormat format, Uint32 sizeInBytes,
              float volume)
{
    if (!dst || !src || (sizeInBytes == 0))
    {
        RAD_LOG_MULTIMEDIA(err, "MixAudio invalid argument(s).");
        return false;
    }
    const bool result = SDL_MixAudio(dst, src, format, sizeInBytes, volume);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_MixAudio failed: {}", SDL_GetError());
    }
    return result;
}

bool ConvertAudioSamples(const SDL_AudioSpec* srcSpec, const Uint8* srcData, int srcSizeInBytes,
                         const SDL_AudioSpec* dstSpec, SDL_Buffer* dstData, int* dstSizeInBytes)
{
    if (!srcSpec || !srcData || (srcSizeInBytes <= 0) || !dstSpec || !dstData || !dstSizeInBytes)
    {
        RAD_LOG_MULTIMEDIA(err, "ConvertAudioSamples invalid argument(s).");
        return false;
    }
    const bool result = SDL_ConvertAudioSamples(srcSpec, srcData, srcSizeInBytes, dstSpec,
                                                &dstData->m_data, dstSizeInBytes);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_ConvertAudioSamples failed: {}", SDL_GetError());
    }
    return result;
}

const char* GetAudioFormatName(SDL_AudioFormat format)
{
    return SDL_GetAudioFormatName(format);
}

int GetSilenceValueForFormat(SDL_AudioFormat format)
{
    return SDL_GetSilenceValueForFormat(format);
}

Ref<AudioStream> AudioStream::Create(const SDL_AudioSpec* srcSpec, const SDL_AudioSpec* dstSpec)
{
    SDL_AudioStream* handle = SDL_CreateAudioStream(srcSpec, dstSpec);
    if (handle)
    {
        return RAD_NEW AudioStream(handle);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_CreateAudioStream failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioStream> AudioStream::OpenDevice(SDL_AudioDeviceID deviceId, const SDL_AudioSpec* spec,
                                         SDL_AudioStreamCallback callback, void* userData)
{
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(deviceId, spec, callback, userData);
    if (stream)
    {
        return RAD_NEW AudioStream(stream);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_OpenAudioDeviceStream failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioStream> AudioStream::OpenDefaultPlayback(const SDL_AudioSpec* spec,
                                                  SDL_AudioStreamCallback callback, void* userData)
{
    return OpenDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, spec, callback, userData);
}

Ref<AudioStream> AudioStream::OpenDefaultRecording(const SDL_AudioSpec* spec,
                                                   SDL_AudioStreamCallback callback, void* userData)
{
    return OpenDevice(SDL_AUDIO_DEVICE_DEFAULT_RECORDING, spec, callback, userData);
}

AudioStream::AudioStream(SDL_AudioStream* handle) :
    m_handle(handle)
{
    m_propertiesId = SDL_GetAudioStreamProperties(m_handle);
    if (m_propertiesId == 0)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioStreamProperties failed: {}", SDL_GetError());
    }
}

AudioStream::~AudioStream()
{
    Destroy();
}

void AudioStream::Destroy()
{
    if (m_handle)
    {
        m_device.reset();
        SDL_DestroyAudioStream(m_handle);
        m_handle = nullptr;
        m_propertiesId = 0;
    }
}

SDL_AudioDeviceID AudioStream::GetDeviceId() const
{
    assert(m_handle != nullptr);
    return SDL_GetAudioStreamDevice(m_handle);
}

bool AudioStream::BindToDevice(Ref<AudioDevice> device)
{
    assert(m_handle != nullptr);
    assert(device != nullptr);

    const bool result = SDL_BindAudioStream(device->GetId(), m_handle);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_BindAudioStream failed: {}", SDL_GetError());
        return false;
    }

    // Keep the device alive for as long as this stream remains bound.
    m_device = std::move(device);
    return result;
}

void AudioStream::UnbindFromDevice()
{
    assert(m_handle != nullptr);
    SDL_UnbindAudioStream(m_handle);
    m_device.reset();
}

bool AudioStream::GetFormat(SDL_AudioSpec* srcSpec, SDL_AudioSpec* dstSpec)
{
    assert(m_handle != nullptr);
    const bool result = SDL_GetAudioStreamFormat(m_handle, srcSpec, dstSpec);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioStreamFormat failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::SetFormat(const SDL_AudioSpec* srcSpec, const SDL_AudioSpec* dstSpec)
{
    assert(m_handle != nullptr);
    const bool result = SDL_SetAudioStreamFormat(m_handle, srcSpec, dstSpec);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioStreamFormat failed: {}", SDL_GetError());
    }
    return result;
}

float AudioStream::GetFrequencyRatio()
{
    assert(m_handle != nullptr);
    return SDL_GetAudioStreamFrequencyRatio(m_handle);
}

bool AudioStream::SetFrequencyRatio(float ratio)
{
    assert(m_handle != nullptr);
    const bool result = SDL_SetAudioStreamFrequencyRatio(m_handle, ratio);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioStreamFrequencyRatio failed: {}", SDL_GetError());
    }
    return result;
}

float AudioStream::GetGain() const
{
    assert(m_handle != nullptr);
    return SDL_GetAudioStreamGain(m_handle);
}

bool AudioStream::SetGain(float gain) const
{
    assert(m_handle != nullptr);
    const bool result = SDL_SetAudioStreamGain(m_handle, gain);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioStreamGain failed: {}", SDL_GetError());
    }
    return result;
}

std::vector<int> AudioStream::GetInputChannelMap() const
{
    std::vector<int> buffer;
    assert(m_handle != nullptr);
    int count = 0;
    int* channelMap = SDL_GetAudioStreamInputChannelMap(m_handle, &count);
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

std::vector<int> AudioStream::GetOutputChannelMap() const
{
    std::vector<int> buffer;
    assert(m_handle != nullptr);
    int count = 0;
    int* channelMap = SDL_GetAudioStreamOutputChannelMap(m_handle, &count);
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

bool AudioStream::SetInputChannelMap(Span<const int> map)
{
    assert(m_handle != nullptr);
    const int count = static_cast<int>(map.size());
    if (count == 0)
    {
        // Passing NULL disables remapping, but count must still match current channel count.
        SDL_AudioSpec src = {};
        SDL_AudioSpec dst = {};
        if (!SDL_GetAudioStreamFormat(m_handle, &src, &dst))
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioStreamFormat failed: {}", SDL_GetError());
            return false;
        }
        return SDL_SetAudioStreamInputChannelMap(m_handle, nullptr, src.channels);
    }

    const bool result = SDL_SetAudioStreamInputChannelMap(m_handle, map.data(), count);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioStreamInputChannelMap failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::SetOutputChannelMap(Span<const int> map)
{
    assert(m_handle != nullptr);
    const int count = static_cast<int>(map.size());
    if (count == 0)
    {
        // Passing NULL disables remapping, but count must still match current channel count.
        SDL_AudioSpec src = {};
        SDL_AudioSpec dst = {};
        if (!SDL_GetAudioStreamFormat(m_handle, &src, &dst))
        {
            RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioStreamFormat failed: {}", SDL_GetError());
            return false;
        }
        return SDL_SetAudioStreamOutputChannelMap(m_handle, nullptr, dst.channels);
    }

    const bool result = SDL_SetAudioStreamOutputChannelMap(m_handle, map.data(), count);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioStreamOutputChannelMap failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::PutData(const void* data, int sizeInBytes)
{
    assert(m_handle != nullptr);
    const bool result = SDL_PutAudioStreamData(m_handle, data, sizeInBytes);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_PutAudioStreamData failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::PutDataNoCopy(const void* data, int sizeInBytes,
                                SDL_AudioStreamDataCompleteCallback callback, void* userData)
{
    assert(m_handle != nullptr);
    const bool result =
        SDL_PutAudioStreamDataNoCopy(m_handle, data, sizeInBytes, callback, userData);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_PutAudioStreamDataNoCopy failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::PutPlanarData(const void* const* data, int numChannels, int numSamples)
{
    assert(m_handle != nullptr);
    const bool result = SDL_PutAudioStreamPlanarData(m_handle, data, numChannels, numSamples);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_PutAudioStreamPlanarData failed: {}", SDL_GetError());
    }
    return result;
}

int AudioStream::GetData(void* data, int sizeInBytes)
{
    assert(m_handle != nullptr);
    const int bytesRead = SDL_GetAudioStreamData(m_handle, data, sizeInBytes);
    if (bytesRead == -1)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioStreamData failed: {}", SDL_GetError());
    }
    return bytesRead;
}

int AudioStream::GetDataSizeAvailable()
{
    assert(m_handle != nullptr);
    const int available = SDL_GetAudioStreamAvailable(m_handle);
    if (available < 0)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioStreamAvailable failed: {}", SDL_GetError());
    }
    return available;
}

int AudioStream::GetDataSizeQueued()
{
    assert(m_handle != nullptr);
    const int queued = SDL_GetAudioStreamQueued(m_handle);
    if (queued < 0)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_GetAudioStreamQueued failed: {}", SDL_GetError());
    }
    return queued;
}

bool AudioStream::Flush()
{
    assert(m_handle != nullptr);
    const bool result = SDL_FlushAudioStream(m_handle);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_FlushAudioStream failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::Clear()
{
    assert(m_handle != nullptr);
    const bool result = SDL_ClearAudioStream(m_handle);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_ClearAudioStream failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::PauseDevice()
{
    assert(m_handle != nullptr);
    const bool result = SDL_PauseAudioStreamDevice(m_handle);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_PauseAudioStreamDevice failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::ResumeDevice()
{
    assert(m_handle != nullptr);
    const bool result = SDL_ResumeAudioStreamDevice(m_handle);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_ResumeAudioStreamDevice failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::IsDevicePaused()
{
    assert(m_handle != nullptr);
    return SDL_AudioStreamDevicePaused(m_handle);
}

bool AudioStream::Lock()
{
    assert(m_handle != nullptr);
    const bool result = SDL_LockAudioStream(m_handle);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_LockAudioStream failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::Unlock()
{
    assert(m_handle != nullptr);
    const bool result = SDL_UnlockAudioStream(m_handle);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_UnlockAudioStream failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::SetGetCallback(SDL_AudioStreamCallback callback, void* userData)
{
    assert(m_handle != nullptr);
    const bool result = SDL_SetAudioStreamGetCallback(m_handle, callback, userData);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioStreamGetCallback failed: {}", SDL_GetError());
    }
    return result;
}

bool AudioStream::SetPutCallback(SDL_AudioStreamCallback callback, void* userData)
{
    assert(m_handle != nullptr);
    const bool result = SDL_SetAudioStreamPutCallback(m_handle, callback, userData);
    if (!result)
    {
        RAD_LOG_MULTIMEDIA(err, "SDL_SetAudioStreamPutCallback failed: {}", SDL_GetError());
    }
    return result;
}

} // namespace rad
