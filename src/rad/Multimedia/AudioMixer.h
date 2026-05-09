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

// Wrapper around MIX_Audio (decoded or generated audio data).
// https://wiki.libsdl.org/SDL3_mixer/CategorySDLMixer
class AudioData : public RefCounted<AudioData>
{
public:
    // Caches SDL_PropertiesID from MIX_GetAudioProperties for metadata accessors.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    AudioData(MIX_Audio* handle);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_DestroyAudio
    ~AudioData();

    // https://wiki.libsdl.org/SDL3_mixer/MIX_Audio
    MIX_Audio* GetHandle() const { return m_handle; }

    // MIX_DURATION_UNKNOWN and MIX_DURATION_INFINITE; see SDL_mixer remarks for MIX_GetAudioDuration.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioDuration
    Sint64 GetDurationInFrames() const;
    // Delegates to GetTimeFromFrameCount(GetDurationInFrames()).
    Sint64 GetDurationInMilliseconds() const
    {
        return GetTimeFromFrameCount(GetDurationInFrames());
    }
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioFormat
    bool GetFormat(SDL_AudioSpec* spec) const;

    // Metadata via SDL_GetStringProperty / SDL_GetNumberProperty / SDL_GetBooleanProperty on MIX_GetAudioProperties.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    const char* GetTitle() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    const char* GetArtist() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    const char* GetAlbum() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    const char* GetCopyright() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    Sint64 GetTrackIndex() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    Sint64 GetTotalTrackCount() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    Sint64 GetYear() const;
    // Same duration as GetDurationInFrames; MIX_PROP_METADATA_DURATION_FRAMES_NUMBER.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    Sint64 GetFrameCount() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioProperties
    bool IsInfinite() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_AudioMSToFrames
    Sint64 GetFrameCountFromTime(Sint64 ms) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_AudioFramesToMS
    Sint64 GetTimeFromFrameCount(Sint64 frames) const;

private:
    MIX_Audio* m_handle;
    SDL_PropertiesID m_props = 0;
}; // class AudioData

// Wrapper around MIX_Track (one mix input / playback slot).
// https://wiki.libsdl.org/SDL3_mixer/CategorySDLMixer
class AudioTrack : public RefCounted<AudioTrack>
{
public:
    // MIX_GetTrackMixer (validates ownership, debug assert), MIX_GetTrackProperties.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackMixer
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackProperties
    AudioTrack(Ref<AudioMixer> mixer, MIX_Track* handle);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_DestroyTrack
    ~AudioTrack();

    // https://wiki.libsdl.org/SDL3_mixer/MIX_Track
    MIX_Track* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackAudio
    bool SetAudio(MIX_Audio* audio);
    // Delegates to SetAudio(MIX_Audio*) using audio->GetHandle(), or nullptr to clear the track.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackAudio
    bool SetAudio(AudioData* audio);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackAudioStream
    bool SetAudioStream(SDL_AudioStream* stream);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackIOStream
    bool SetIOStream(SDL_IOStream* io, bool closeIO);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackRawIOStream
    bool SetRawIOStream(SDL_IOStream* io, const SDL_AudioSpec* spec, bool closeIO);

    // https://wiki.libsdl.org/SDL3_mixer/MIX_TagTrack
    bool Tag(cstring_view tag) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_UntagTrack
    void Untag(cstring_view tag) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackTags
    std::vector<std::string> GetTags() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackPlaybackPosition
    bool SetPlaybackPosition(Sint64 frames) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackPlaybackPosition
    Sint64 GetPlaybackPosition() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackFadeFrames
    Sint64 GetFadeFrames() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackLoops
    bool IsLooping() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackLoops
    int GetLoops() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackLoops
    bool SetLoops(int loops) const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackStoppedCallback
    bool SetStoppedCallback(MIX_TrackStoppedCallback callback, void* userData) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackRawCallback
    bool SetRawCallback(MIX_TrackMixCallback callback, void* userData) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackCookedCallback
    bool SetCookedCallback(MIX_TrackMixCallback callback, void* userData) const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackGroup
    bool SetGroup(AudioTrackGroup* group) const; // nullptr to use mixer's default group

    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackAudio
    MIX_Audio* GetTrackAudio() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackAudioStream
    SDL_AudioStream* GetTrackAudioStream() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackRemaining
    Sint64 GetRemainingFrames() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_TrackMSToFrames
    Sint64 GetFrameCountFromTime(Milliseconds ms) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_TrackFramesToMS
    Milliseconds GetTimeFromFrameCount(Sint64 frames) const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_PlayTrack
    bool Play(SDL_PropertiesID options) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_StopTrack
    bool Stop(Sint64 fadeOutFrames) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_PauseTrack
    bool Pause() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_ResumeTrack
    bool Resume() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_TrackPlaying
    bool IsPlaying() const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_TrackPaused
    bool IsPaused() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackGain
    bool SetGain(float gain) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackGain
    float GetGain() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackFrequencyRatio
    bool SetFrequencyRatio(float ratio) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrackFrequencyRatio
    float GetFrequencyRatio() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackOutputChannelMap
    bool SetOutputChannelMap(const int* chmap, int count) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrackStereo
    bool SetStereo(const MIX_StereoGains* gains) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTrack3DPosition
    bool Set3DPosition(const MIX_Point3D* position) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTrack3DPosition
    bool Get3DPosition(MIX_Point3D* position) const;

    Ref<AudioMixer> m_mixer;
    MIX_Track* m_handle;
    SDL_PropertiesID m_propertiesId = 0;

}; // class AudioTrack

// Wrapper around MIX_Group (track grouping; optional group post-mix callback).
// https://wiki.libsdl.org/SDL3_mixer/CategorySDLMixer
class AudioTrackGroup : public RefCounted<AudioTrackGroup>
{
public:
    // MIX_GetGroupMixer (validates ownership, debug assert), MIX_GetGroupProperties.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetGroupMixer
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetGroupProperties
    AudioTrackGroup(Ref<AudioMixer> mixer, MIX_Group* handle);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_DestroyGroup
    ~AudioTrackGroup();

    // https://wiki.libsdl.org/SDL3_mixer/MIX_Group
    MIX_Group* GetHandle() const { return m_handle; }
    // Cached from MIX_GetGroupProperties in the constructor.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetGroupProperties
    SDL_PropertiesID GetPropertiesId() const { return m_propertiesId; }

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetGroupPostMixCallback
    bool SetPostMixCallback(MIX_GroupMixCallback callback, void* userData) const;

private:
    Ref<AudioMixer> m_mixer;
    MIX_Group* m_handle = nullptr;
    SDL_PropertiesID m_propertiesId = 0;
}; // class AudioTrackGroup

// Wrapper around MIX_Mixer (SDL_mixer 3 mixing engine).
// https://wiki.libsdl.org/SDL3_mixer/CategorySDLMixer
class AudioMixer : public RefCounted<AudioMixer>
{
public:
    using AudioTrackHandle = MIX_Track*;

    // MIX_Init, MIX_CreateMixerDevice, MIX_GetMixerProperties (failure path calls MIX_Quit).
    // https://wiki.libsdl.org/SDL3_mixer/MIX_Init
    // https://wiki.libsdl.org/SDL3_mixer/MIX_CreateMixerDevice
    static Ref<AudioMixer> Create(SDL_AudioDeviceID deviceId, const SDL_AudioSpec* spec);
    // MIX_Init, MIX_CreateMixer (no device; output via MIX_Generate; failure path calls MIX_Quit).
    // https://wiki.libsdl.org/SDL3_mixer/MIX_Init
    // https://wiki.libsdl.org/SDL3_mixer/MIX_CreateMixer
    static Ref<AudioMixer> Create(const SDL_AudioSpec* spec);

    // MIX_DestroyMixer; MIX_Quit balances MIX_Init from the static Create overloads.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_DestroyMixer
    // https://wiki.libsdl.org/SDL3_mixer/MIX_Quit
    ~AudioMixer();

    // https://wiki.libsdl.org/SDL3_mixer/MIX_Mixer
    MIX_Mixer* GetHandle() const { return m_handle; }
    // Cached from MIX_Version during construction.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_Version
    int GetVersion() const { return m_version; }
    // Cached from MIX_GetMixerProperties in the private constructor.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetMixerProperties
    SDL_PropertiesID GetPropertiesId() const { return m_propertiesId; }
    // Cached MIX_PROP_MIXER_DEVICE_NUMBER from mixer properties (0 for memory mixers).
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetMixerProperties
    SDL_AudioDeviceID GetDeviceId() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_LockMixer
    void Lock();
    // https://wiki.libsdl.org/SDL3_mixer/MIX_UnlockMixer
    void Unlock();

    // Decoder names cached at construction (MIX_GetNumAudioDecoders / MIX_GetAudioDecoder).
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetNumAudioDecoders
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetAudioDecoder
    bool HasDecoder(std::string_view decoderName) const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetMixerFormat
    bool GetFormat(SDL_AudioSpec* spec) const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_LoadAudio_IO
    Ref<AudioData> LoadAudioIO(SDL_IOStream* io, bool predecode, bool closeIO);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_LoadAudio
    Ref<AudioData> LoadAudio(cstring_view path, bool predecode);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_LoadAudioWithProperties
    Ref<AudioData> LoadAudioWithProperties(SDL_PropertiesID props);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_LoadAudioNoCopy
    Ref<AudioData> LoadAudioNoCopy(const void* data, size_t dataSize, bool freeWhenDone);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_LoadRawAudio_IO
    Ref<AudioData> LoadRawAudioIO(SDL_IOStream* io, const SDL_AudioSpec* spec, bool closeIO);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_LoadRawAudio
    Ref<AudioData> LoadRawAudio(const void* data, size_t dataSize, const SDL_AudioSpec* spec);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_LoadRawAudioNoCopy
    Ref<AudioData> LoadRawAudioNoCopy(const void* data, size_t dataSize, const SDL_AudioSpec* spec);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_CreateSineWaveAudio
    Ref<AudioData> CreateSineWaveAudio(int hz, float amplitude, Sint64 ms);

    // https://wiki.libsdl.org/SDL3_mixer/MIX_CreateTrack
    Ref<AudioTrack> CreateTrack();
    // https://wiki.libsdl.org/SDL3_mixer/MIX_CreateGroup
    Ref<AudioTrackGroup> CreateGroup();

    // https://wiki.libsdl.org/SDL3_mixer/MIX_PlayTag
    bool PlayTag(cstring_view tag, SDL_PropertiesID options);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_PlayAudio
    bool PlayAudio(MIX_Audio* audio);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_StopAllTracks
    bool StopAllTracks(Milliseconds fadeOut);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_StopTag
    bool StopTag(cstring_view tag, Milliseconds fadeOut);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_PauseAllTracks
    bool PauseAllTracks();
    // https://wiki.libsdl.org/SDL3_mixer/MIX_PauseTag
    bool PauseTag(cstring_view tag);
    // https://wiki.libsdl.org/SDL3_mixer/MIX_ResumeAllTracks
    bool ResumeAllTracks();
    // https://wiki.libsdl.org/SDL3_mixer/MIX_ResumeTag
    bool ResumeTag(cstring_view tag);

    // Master gain for the whole mixer (SDL_mixer term: mixer gain).
    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetMixerGain
    bool SetMasterGain(float gain) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetMixerGain
    float GetMasterGain() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetMixerFrequencyRatio
    bool SetFrequencyRatio(float ratio) const;
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetMixerFrequencyRatio
    float GetFrequencyRatio() const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetTagGain
    bool SetTagGain(cstring_view tag, float gain) const;

    // Returned track handles are owned by SDL_mixer; do not destroy them.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetTaggedTracks
    std::vector<AudioTrackHandle> GetTaggedTrackHandles(cstring_view tag) const;

    // https://wiki.libsdl.org/SDL3_mixer/MIX_SetPostMixCallback
    bool SetPostMixCallback(MIX_PostMixCallback callback, void* userData) const;
    // Memory mixers only (MIX_CreateMixer); MIX_Generate fills buffer with mixed output.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_Generate
    int Generate(void* buffer, int bufferSize) const;

private:
    // MIX_GetMixerProperties, MIX_Version, MIX_GetNumAudioDecoders / MIX_GetAudioDecoder.
    // https://wiki.libsdl.org/SDL3_mixer/MIX_GetMixerProperties
    explicit AudioMixer(MIX_Mixer* mixerHandle);

    bool m_mixInitialized = false;
    int m_version = 0;
    std::vector<std::string> m_decoders;
    MIX_Mixer* m_handle = nullptr;
    SDL_PropertiesID m_propertiesId = 0;
    SDL_AudioDeviceID m_deviceId = 0;
}; // class AudioMixer

} // namespace rad
