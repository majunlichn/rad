#pragma once

#include <rad/Multimedia/AudioDevice.h>
#include <rad/Common/String.h>

#include <SDL3_mixer/SDL_mixer.h>

#include <vector>
#include <string_view>

namespace rad
{

class AudioMixer;
class AudioTrackGroup;

class AudioData : public RefCounted<AudioData>
{
public:
    AudioData(MIX_Audio* handle);
    ~AudioData();

    MIX_Audio* GetHandle() const { return m_handle; }

    // MIX_DURATION_UNKNOWN
    // MIX_DURATION_INFINITE
    Sint64 GetDurationInFrames() const;
    Sint64 GetDurationInMilliseconds() const
    {
        return GetTimeFromFrameCount(GetDurationInFrames());
    }
    bool GetFormat(SDL_AudioSpec* spec) const;

    const char* GetTitle() const;
    const char* GetArtist() const;
    const char* GetAlbum() const;
    const char* GetCopyright() const;
    Sint64 GetTrackIndex() const;
    Sint64 GetTotalTrackCount() const;
    Sint64 GetYear() const;
    // Same as GetDurationInFrames.
    Sint64 GetFrameCount() const;
    bool IsInfinite() const;

    Sint64 GetFrameCountFromTime(Sint64 ms) const;
    Sint64 GetTimeFromFrameCount(Sint64 frames) const;

private:
    MIX_Audio* m_handle;
    SDL_PropertiesID m_props = 0;
}; // class AudioData

class AudioTrack : public RefCounted<AudioTrack>
{
public:
    AudioTrack(Ref<AudioMixer> mixer, MIX_Track* handle);
    ~AudioTrack();

    MIX_Track* GetHandle() const { return m_handle; }

    bool SetAudio(MIX_Audio* audio);
    bool SetAudio(AudioData* audio);
    bool SetAudioStream(SDL_AudioStream* stream);
    bool SetIOStream(SDL_IOStream* io, bool closeIO);
    bool SetRawIOStream(SDL_IOStream* io, const SDL_AudioSpec* spec, bool closeIO);

    bool Tag(cstring_view tag) const;
    void Untag(cstring_view tag) const;
    std::vector<std::string> GetTags() const;

    bool SetPlaybackPosition(Sint64 frames) const;
    Sint64 GetPlaybackPosition() const;
    Sint64 GetFadeFrames() const;

    bool IsLooping() const;
    int GetLoops() const;
    bool SetLoops(int loops) const;

    bool SetStoppedCallback(MIX_TrackStoppedCallback callback, void* userData) const;
    bool SetRawCallback(MIX_TrackMixCallback callback, void* userData) const;
    bool SetCookedCallback(MIX_TrackMixCallback callback, void* userData) const;

    bool SetGroup(AudioTrackGroup* group) const; // nullptr to use mixer's default group

    MIX_Audio* GetTrackAudio() const;
    SDL_AudioStream* GetTrackAudioStream() const;

    Sint64 GetRemainingFrames() const;
    Sint64 GetFrameCountFromTime(Milliseconds ms) const;
    Milliseconds GetTimeFromFrameCount(Sint64 frames) const;

    bool Play(SDL_PropertiesID options) const;
    bool Stop(Sint64 fadeOutFrames) const;
    bool Pause() const;
    bool Resume() const;

    bool IsPlaying() const;
    bool IsPaused() const;

    bool SetGain(float gain) const;
    float GetGain() const;

    bool SetFrequencyRatio(float ratio) const;
    float GetFrequencyRatio() const;

    bool SetOutputChannelMap(const int* chmap, int count) const;
    bool SetStereo(const MIX_StereoGains* gains) const;
    bool Set3DPosition(const MIX_Point3D* position) const;
    bool Get3DPosition(MIX_Point3D* position) const;

    Ref<AudioMixer> m_mixer;
    MIX_Track* m_handle;
    SDL_PropertiesID m_propertiesId = 0;

}; // class AudioTrack

class AudioTrackGroup : public RefCounted<AudioTrackGroup>
{
public:
    AudioTrackGroup(Ref<AudioMixer> mixer, MIX_Group* handle);
    ~AudioTrackGroup();

    MIX_Group* GetHandle() const { return m_handle; }
    SDL_PropertiesID GetPropertiesId() const { return m_propertiesId; }

    bool SetPostMixCallback(MIX_GroupMixCallback callback, void* userData) const;

private:
    Ref<AudioMixer> m_mixer;
    MIX_Group* m_handle = nullptr;
    SDL_PropertiesID m_propertiesId = 0;
}; // class AudioTrackGroup

class AudioMixer : public RefCounted<AudioMixer>
{
public:
    using AudioTrackHandle = MIX_Track*;

    static Ref<AudioMixer> Create(SDL_AudioDeviceID deviceId, const SDL_AudioSpec* spec);
    static Ref<AudioMixer> Create(const SDL_AudioSpec* spec);

    ~AudioMixer();

    MIX_Mixer* GetHandle() const { return m_handle; }
    int GetVersion() const { return m_version; }
    SDL_PropertiesID GetPropertiesId() const { return m_propertiesId; }
    SDL_AudioDeviceID GetDeviceId() const;

    void Lock();
    void Unlock();

    bool HasDecoder(std::string_view decoderName) const;

    bool GetFormat(SDL_AudioSpec* spec) const;

    Ref<AudioData> LoadAudioIO(SDL_IOStream* io, bool predecode, bool closeIO);
    Ref<AudioData> LoadAudio(cstring_view path, bool predecode);
    Ref<AudioData> LoadAudioWithProperties(SDL_PropertiesID props);
    Ref<AudioData> LoadAudioNoCopy(const void* data, size_t dataSize, bool freeWhenDone);
    Ref<AudioData> LoadRawAudioIO(SDL_IOStream* io, const SDL_AudioSpec* spec, bool closeIO);
    Ref<AudioData> LoadRawAudio(const void* data, size_t dataSize, const SDL_AudioSpec* spec);
    Ref<AudioData> LoadRawAudioNoCopy(const void* data, size_t dataSize, const SDL_AudioSpec* spec);
    Ref<AudioData> CreateSineWaveAudio(int hz, float amplitude, Sint64 ms);

    Ref<AudioTrack> CreateTrack();
    Ref<AudioTrackGroup> CreateGroup();

    bool PlayTag(cstring_view tag, SDL_PropertiesID options);
    bool PlayAudio(MIX_Audio* audio);
    bool StopAllTracks(Milliseconds fadeOut);
    bool StopTag(cstring_view tag, Milliseconds fadeOut);
    bool PauseAllTracks();
    bool PauseTag(cstring_view tag);
    bool ResumeAllTracks();
    bool ResumeTag(cstring_view tag);

    // Master gain for the whole mixer (SDL_mixer calls this "mixer gain").
    bool SetMasterGain(float gain) const;
    float GetMasterGain() const;

    bool SetFrequencyRatio(float ratio) const;
    float GetFrequencyRatio() const;

    bool SetTagGain(cstring_view tag, float gain) const;

    // Returned track handles are owned by SDL_mixer; do not destroy them.
    std::vector<AudioTrackHandle> GetTaggedTrackHandles(cstring_view tag) const;

    bool SetPostMixCallback(MIX_PostMixCallback callback, void* userData) const;
    // Generate mixed audio when not driving an audio device.
    // Returns number of bytes written, or -1 on error.
    int Generate(void* buffer, int bufferSize) const;

private:
    explicit AudioMixer(MIX_Mixer* mixerHandle);

    bool m_mixInitialized = false;
    int m_version = 0;
    std::vector<std::string> m_decoders;
    MIX_Mixer* m_handle = nullptr;
    SDL_PropertiesID m_propertiesId = 0;
    SDL_AudioDeviceID m_deviceId = 0;
}; // class AudioMixer

} // namespace rad
