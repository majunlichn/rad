#include <rad/Common/MemoryDebug.h>

#include <rad/Multimedia/AudioMixer.h>

namespace rad
{

AudioData::AudioData(MIX_Audio* handle) :
    m_handle(handle)
{
    assert(m_handle);
    if (m_handle)
    {
        m_props = MIX_GetAudioProperties(m_handle);
        if (!m_props)
        {
            RAD_LOG_MULTIMEDIA(err, "MIX_GetAudioProperties failed: {}", SDL_GetError());
        }
    }
}

AudioData::~AudioData()
{
    if (m_handle)
    {
        MIX_DestroyAudio(m_handle);
        m_handle = nullptr;
    }
}

Sint64 AudioData::GetDurationInFrames() const
{
    return MIX_GetAudioDuration(m_handle);
}

bool AudioData::GetFormat(SDL_AudioSpec* spec) const
{
    return MIX_GetAudioFormat(m_handle, spec);
}

const char* AudioData::GetTitle() const
{
    return SDL_GetStringProperty(m_props, MIX_PROP_METADATA_TITLE_STRING, "");
}

const char* AudioData::GetArtist() const
{
    return SDL_GetStringProperty(m_props, MIX_PROP_METADATA_ARTIST_STRING, "");
}

const char* AudioData::GetAlbum() const
{
    return SDL_GetStringProperty(m_props, MIX_PROP_METADATA_ALBUM_STRING, "");
}

const char* AudioData::GetCopyright() const
{
    return SDL_GetStringProperty(m_props, MIX_PROP_METADATA_COPYRIGHT_STRING, "");
}

Sint64 AudioData::GetTrackIndex() const
{
    return SDL_GetNumberProperty(m_props, MIX_PROP_METADATA_TRACK_NUMBER, 0);
}

Sint64 AudioData::GetTotalTrackCount() const
{
    return SDL_GetNumberProperty(m_props, MIX_PROP_METADATA_TOTAL_TRACKS_NUMBER, 0);
}

Sint64 AudioData::GetYear() const
{
    return SDL_GetNumberProperty(m_props, MIX_PROP_METADATA_YEAR_NUMBER, 0);
}

Sint64 AudioData::GetFrameCount() const
{
    return SDL_GetNumberProperty(m_props, MIX_PROP_METADATA_DURATION_FRAMES_NUMBER, 0);
}

bool AudioData::IsInfinite() const
{
    return SDL_GetBooleanProperty(m_props, MIX_PROP_METADATA_DURATION_INFINITE_BOOLEAN, false);
}

Sint64 AudioData::GetFrameCountFromTime(Sint64 ms) const
{
    return MIX_AudioMSToFrames(m_handle, ms);
}

Sint64 AudioData::GetTimeFromFrameCount(Sint64 frames) const
{
    return MIX_AudioFramesToMS(m_handle, frames);
}

AudioTrack::AudioTrack(Ref<AudioMixer> mixer, MIX_Track* handle) :
    m_mixer(std::move(mixer)),
    m_handle(handle)
{
    assert(m_handle);
    if (m_handle)
    {
        MIX_Mixer* mixerHandle = MIX_GetTrackMixer(m_handle);
        assert(mixerHandle == m_mixer->GetHandle());

        m_propertiesId = MIX_GetTrackProperties(m_handle);
        if (!m_propertiesId)
        {
            RAD_LOG_MULTIMEDIA(err, "MIX_GetTrackProperties failed: {}", SDL_GetError());
        }
    }
}

AudioTrack::~AudioTrack()
{
    if (m_handle)
    {
        MIX_DestroyTrack(m_handle);
        m_handle = nullptr;
    }
}

bool AudioTrack::SetAudio(MIX_Audio* audio)
{
    return MIX_SetTrackAudio(m_handle, audio);
}

bool AudioTrack::SetAudio(AudioData* audio)
{
    // Treat nullptr as "clear track input" (mirrors passing nullptr to MIX_SetTrackAudio).
    return SetAudio(audio ? audio->GetHandle() : nullptr);
}

bool AudioTrack::SetAudioStream(SDL_AudioStream* stream)
{
    return MIX_SetTrackAudioStream(m_handle, stream);
}

bool AudioTrack::SetIOStream(SDL_IOStream* io, bool closeIO)
{
    return MIX_SetTrackIOStream(m_handle, io, closeIO);
}

bool AudioTrack::SetRawIOStream(SDL_IOStream* io, const SDL_AudioSpec* spec, bool closeIO)
{
    return MIX_SetTrackRawIOStream(m_handle, io, spec, closeIO);
}

bool AudioTrack::Tag(cstring_view tag) const
{
    return MIX_TagTrack(m_handle, tag.c_str());
}

void AudioTrack::Untag(cstring_view tag) const
{
    MIX_UntagTrack(m_handle, tag.c_str());
}

std::vector<std::string> AudioTrack::GetTags() const
{
    int count = 0;
    char** tags = MIX_GetTrackTags(m_handle, &count);
    if (!tags)
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_GetTrackTags failed: {}", SDL_GetError());
        return {};
    }

    std::vector<std::string> out;
    out.reserve(count > 0 ? static_cast<size_t>(count) : 0u);
    for (int i = 0; i < count; ++i)
    {
        if (tags[i])
        {
            out.emplace_back(tags[i]);
        }
    }

    SDL_free(tags);
    return out;
}

bool AudioTrack::SetPlaybackPosition(Sint64 frames) const
{
    return MIX_SetTrackPlaybackPosition(m_handle, frames);
}

Sint64 AudioTrack::GetPlaybackPosition() const
{
    return MIX_GetTrackPlaybackPosition(m_handle);
}

Sint64 AudioTrack::GetFadeFrames() const
{
    return MIX_GetTrackFadeFrames(m_handle);
}

bool AudioTrack::IsLooping() const
{
    return MIX_GetTrackLoops(m_handle) != 0;
}

int AudioTrack::GetLoops() const
{
    return MIX_GetTrackLoops(m_handle);
}

bool AudioTrack::SetLoops(int loops) const
{
    return MIX_SetTrackLoops(m_handle, loops);
}

bool AudioTrack::SetStoppedCallback(MIX_TrackStoppedCallback callback, void* userData) const
{
    return MIX_SetTrackStoppedCallback(m_handle, callback, userData);
}

bool AudioTrack::SetRawCallback(MIX_TrackMixCallback callback, void* userData) const
{
    return MIX_SetTrackRawCallback(m_handle, callback, userData);
}

bool AudioTrack::SetCookedCallback(MIX_TrackMixCallback callback, void* userData) const
{
    return MIX_SetTrackCookedCallback(m_handle, callback, userData);
}

bool AudioTrack::SetGroup(AudioTrackGroup* group) const
{
    return MIX_SetTrackGroup(m_handle, group ? group->GetHandle() : nullptr);
}

MIX_Audio* AudioTrack::GetTrackAudio() const
{
    return MIX_GetTrackAudio(m_handle);
}

SDL_AudioStream* AudioTrack::GetTrackAudioStream() const
{
    return MIX_GetTrackAudioStream(m_handle);
}

Sint64 AudioTrack::GetRemainingFrames() const
{
    return MIX_GetTrackRemaining(m_handle);
}

Sint64 AudioTrack::GetFrameCountFromTime(Milliseconds ms) const
{
    return MIX_TrackMSToFrames(m_handle, static_cast<Sint64>(ms.count()));
}

Milliseconds AudioTrack::GetTimeFromFrameCount(Sint64 frames) const
{
    return Milliseconds(MIX_TrackFramesToMS(m_handle, frames));
}

bool AudioTrack::Play(SDL_PropertiesID options) const
{
    return MIX_PlayTrack(m_handle, options);
}

bool AudioTrack::Stop(Sint64 fadeOutFrames) const
{
    return MIX_StopTrack(m_handle, fadeOutFrames);
}

bool AudioTrack::Pause() const
{
    return MIX_PauseTrack(m_handle);
}

bool AudioTrack::Resume() const
{
    return MIX_ResumeTrack(m_handle);
}

bool AudioTrack::IsPlaying() const
{
    return MIX_TrackPlaying(m_handle);
}

bool AudioTrack::IsPaused() const
{
    return MIX_TrackPaused(m_handle);
}

bool AudioTrack::SetGain(float gain) const
{
    return MIX_SetTrackGain(m_handle, gain);
}

float AudioTrack::GetGain() const
{
    return MIX_GetTrackGain(m_handle);
}

bool AudioTrack::SetFrequencyRatio(float ratio) const
{
    return MIX_SetTrackFrequencyRatio(m_handle, ratio);
}

float AudioTrack::GetFrequencyRatio() const
{
    return MIX_GetTrackFrequencyRatio(m_handle);
}

bool AudioTrack::SetOutputChannelMap(const int* chmap, int count) const
{
    return MIX_SetTrackOutputChannelMap(m_handle, chmap, count);
}

bool AudioTrack::SetStereo(const MIX_StereoGains* gains) const
{
    return MIX_SetTrackStereo(m_handle, gains);
}

bool AudioTrack::Set3DPosition(const MIX_Point3D* position) const
{
    return MIX_SetTrack3DPosition(m_handle, position);
}

bool AudioTrack::Get3DPosition(MIX_Point3D* position) const
{
    return MIX_GetTrack3DPosition(m_handle, position);
}

Ref<AudioMixer> AudioMixer::Create(SDL_AudioDeviceID deviceId, const SDL_AudioSpec* spec)
{
    if (!MIX_Init())
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_Init failed: {}", SDL_GetError());
        return nullptr;
    }

    MIX_Mixer* mixerHandle = MIX_CreateMixerDevice(deviceId, spec);
    if (!mixerHandle)
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_CreateMixerDevice failed: {}", SDL_GetError());
        MIX_Quit();
        return nullptr;
    }

    return RAD_NEW AudioMixer(mixerHandle);
}

Ref<AudioMixer> AudioMixer::Create(const SDL_AudioSpec* spec)
{
    if (!MIX_Init())
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_Init failed: {}", SDL_GetError());
        return nullptr;
    }

    MIX_Mixer* mixerHandle = MIX_CreateMixer(spec);
    if (!mixerHandle)
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_CreateMixer failed: {}", SDL_GetError());
        MIX_Quit();
        return nullptr;
    }

    // Use the private constructor so the destructor will clean up correctly.
    return RAD_NEW AudioMixer(mixerHandle);
}

AudioMixer::AudioMixer(MIX_Mixer* handle) :
    m_handle(handle)
{
    // Factories call MIX_Init() already.
    m_mixInitialized = true;
    m_version = MIX_Version();

    int decoderCount = MIX_GetNumAudioDecoders();
    m_decoders.resize(decoderCount);
    for (int i = 0; i < decoderCount; ++i)
    {
        m_decoders[i] = MIX_GetAudioDecoder(i);
    }

    if (m_handle)
    {
        m_propertiesId = MIX_GetMixerProperties(m_handle);
        if (!m_propertiesId)
        {
            RAD_LOG_MULTIMEDIA(err, "MIX_GetMixerProperties failed: {}", SDL_GetError());
        }
        m_deviceId = SDL_GetNumberProperty(m_propertiesId, MIX_PROP_MIXER_DEVICE_NUMBER, 0);
    }
}

AudioMixer::~AudioMixer()
{
    if (m_handle)
    {
        MIX_DestroyMixer(m_handle);
    }

    if (m_mixInitialized)
    {
        MIX_Quit();
    }
}

bool AudioMixer::HasDecoder(std::string_view decoderName) const
{
    return std::find(m_decoders.begin(), m_decoders.end(), decoderName) != m_decoders.end();
}

SDL_AudioDeviceID AudioMixer::GetDeviceId() const
{
    return m_deviceId;
}

bool AudioMixer::GetFormat(SDL_AudioSpec* spec) const
{
    return MIX_GetMixerFormat(m_handle, spec);
}

Ref<AudioData> AudioMixer::LoadAudioIO(SDL_IOStream* io, bool predecode, bool closeIO)
{
    MIX_Audio* audio = MIX_LoadAudio_IO(m_handle, io, predecode, closeIO);
    if (audio)
    {
        return RAD_NEW AudioData(audio);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_LoadAudio_IO failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioData> AudioMixer::LoadAudio(cstring_view path, bool predecode)
{
    MIX_Audio* audio = MIX_LoadAudio(m_handle, path.c_str(), predecode);
    if (audio)
    {
        return RAD_NEW AudioData(audio);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_LoadAudio failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioData> AudioMixer::LoadAudioWithProperties(SDL_PropertiesID props)
{
    MIX_Audio* audio = MIX_LoadAudioWithProperties(props);
    if (audio)
    {
        return RAD_NEW AudioData(audio);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_LoadAudioWithProperties failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioData> AudioMixer::LoadAudioNoCopy(const void* data, size_t dataSize, bool freeWhenDone)
{
    MIX_Audio* audio = MIX_LoadAudioNoCopy(m_handle, data, dataSize, freeWhenDone);
    if (audio)
    {
        return RAD_NEW AudioData(audio);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_LoadAudioNoCopy failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioData> AudioMixer::LoadRawAudioIO(SDL_IOStream* io, const SDL_AudioSpec* spec, bool closeIO)
{
    MIX_Audio* audio = MIX_LoadRawAudio_IO(m_handle, io, spec, closeIO);
    if (audio)
    {
        return RAD_NEW AudioData(audio);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_LoadRawAudio_IO failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioData> AudioMixer::LoadRawAudio(const void* data, size_t dataSize,
                                        const SDL_AudioSpec* spec)
{
    MIX_Audio* audio = MIX_LoadRawAudio(m_handle, data, dataSize, spec);
    if (audio)
    {
        return RAD_NEW AudioData(audio);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_LoadRawAudio failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioData> AudioMixer::LoadRawAudioNoCopy(const void* data, size_t dataSize,
                                              const SDL_AudioSpec* spec)
{
    MIX_Audio* audio = MIX_LoadRawAudioNoCopy(m_handle, data, dataSize, spec, false);
    if (audio)
    {
        return RAD_NEW AudioData(audio);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_LoadRawAudioNoCopy failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioData> AudioMixer::CreateSineWaveAudio(int hz, float amplitude, Sint64 ms)
{
    MIX_Audio* audio = MIX_CreateSineWaveAudio(m_handle, hz, amplitude, ms);
    if (audio)
    {
        return RAD_NEW AudioData(audio);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_CreateSineWaveAudio failed: {}", SDL_GetError());
        return nullptr;
    }
}

Ref<AudioTrack> AudioMixer::CreateTrack()
{
    MIX_Track* track = MIX_CreateTrack(m_handle);
    if (track)
    {
        return RAD_NEW AudioTrack(this, track);
    }
    else
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_CreateTrack failed: {}", SDL_GetError());
        return nullptr;
    }
}

AudioTrackGroup::AudioTrackGroup(Ref<AudioMixer> mixer, MIX_Group* handle) :
    m_mixer(std::move(mixer)),
    m_handle(handle)
{
    if (m_handle)
    {
        MIX_Mixer* mixerHandle = MIX_GetGroupMixer(m_handle);
        assert(mixerHandle == m_mixer->GetHandle());

        m_propertiesId = MIX_GetGroupProperties(m_handle);
        if (!m_propertiesId)
        {
            RAD_LOG_MULTIMEDIA(err, "MIX_GetGroupProperties failed: {}", SDL_GetError());
        }
    }
}

AudioTrackGroup::~AudioTrackGroup()
{
    if (m_handle)
    {
        MIX_DestroyGroup(m_handle);
    }
}

bool AudioTrackGroup::SetPostMixCallback(MIX_GroupMixCallback callback, void* userData) const
{
    return MIX_SetGroupPostMixCallback(m_handle, callback, userData);
}

Ref<AudioTrackGroup> AudioMixer::CreateGroup()
{
    MIX_Group* group = MIX_CreateGroup(m_handle);
    if (!group)
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_CreateGroup failed: {}", SDL_GetError());
        return nullptr;
    }
    return RAD_NEW AudioTrackGroup(this, group);
}

void AudioMixer::Lock()
{
    MIX_LockMixer(m_handle);
}

void AudioMixer::Unlock()
{
    MIX_UnlockMixer(m_handle);
}

bool AudioMixer::PlayTag(cstring_view tag, SDL_PropertiesID options)
{
    return MIX_PlayTag(m_handle, tag.c_str(), options);
}

bool AudioMixer::PlayAudio(MIX_Audio* audio)
{
    return MIX_PlayAudio(m_handle, audio);
}

bool AudioMixer::StopAllTracks(Milliseconds fadeOut)
{
    return MIX_StopAllTracks(m_handle, static_cast<Sint64>(fadeOut.count()));
}

bool AudioMixer::StopTag(cstring_view tag, Milliseconds fadeOut)
{
    return MIX_StopTag(m_handle, tag.c_str(), static_cast<Sint64>(fadeOut.count()));
}

bool AudioMixer::PauseAllTracks()
{
    return MIX_PauseAllTracks(m_handle);
}

bool AudioMixer::PauseTag(cstring_view tag)
{
    return MIX_PauseTag(m_handle, tag.c_str());
}

bool AudioMixer::ResumeAllTracks()
{
    return MIX_ResumeAllTracks(m_handle);
}

bool AudioMixer::ResumeTag(cstring_view tag)
{
    return MIX_ResumeTag(m_handle, tag.c_str());
}

bool AudioMixer::SetTagGain(cstring_view tag, float gain) const
{
    return MIX_SetTagGain(m_handle, tag.c_str(), gain);
}

bool AudioMixer::SetMasterGain(float gain) const
{
    return MIX_SetMixerGain(m_handle, gain);
}

float AudioMixer::GetMasterGain() const
{
    return MIX_GetMixerGain(m_handle);
}

bool AudioMixer::SetFrequencyRatio(float ratio) const
{
    return MIX_SetMixerFrequencyRatio(m_handle, ratio);
}

float AudioMixer::GetFrequencyRatio() const
{
    return MIX_GetMixerFrequencyRatio(m_handle);
}

bool AudioMixer::SetPostMixCallback(MIX_PostMixCallback cb, void* userData) const
{
    return MIX_SetPostMixCallback(m_handle, cb, userData);
}

std::vector<AudioMixer::AudioTrackHandle> AudioMixer::GetTaggedTrackHandles(cstring_view tag) const
{
    int count = 0;
    MIX_Track** tracks = MIX_GetTaggedTracks(m_handle, tag.c_str(), &count);
    if (!tracks)
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_GetTaggedTracks failed: {}", SDL_GetError());
        return {};
    }

    std::vector<AudioTrackHandle> out;
    out.reserve(count > 0 ? static_cast<size_t>(count) : 0u);
    for (int i = 0; i < count; ++i)
    {
        if (tracks[i])
        {
            out.push_back(tracks[i]);
        }
    }

    SDL_free(tracks);
    return out;
}

int AudioMixer::Generate(void* buffer, int bufferSize) const
{
    if (m_deviceId)
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_Generate is only valid for memory mixers (MIX_CreateMixer).");
        return -1;
    }

    const int result = MIX_Generate(m_handle, buffer, bufferSize);
    if (result < 0)
    {
        RAD_LOG_MULTIMEDIA(err, "MIX_Generate failed: {}", SDL_GetError());
    }
    return result;
}

} // namespace rad
