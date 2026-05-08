#include <rad/Multimedia/AudioDevice.h>

#include "Multimedia.test.h"

TEST(Audio, EnumerateDrivers)
{
    auto& audioSystem = rad::AudioSystem::GetInstance();
    ASSERT_TRUE(audioSystem.Init());
    auto drivers = audioSystem.GetAudioDrivers();
    for (size_t i = 0; i < drivers.size(); ++i)
    {
        RAD_LOG_MULTIMEDIA(info, "AudioDriver#{}: {}", i, drivers[i]);
    }
    RAD_LOG_MULTIMEDIA(info, "Current AudioDriver: {}", audioSystem.GetCurrentAudioDriver());
    audioSystem.Shutdown();
}

TEST(AudioDevice, EnumerateDevices)
{
    auto& audioSystem = rad::AudioSystem::GetInstance();
    ASSERT_TRUE(audioSystem.Init());
    auto playbackDevices = audioSystem.GetAudioPlaybackDevices();
    for (size_t i = 0; i < playbackDevices.size(); ++i)
    {
        auto& device = playbackDevices[i];
        const SDL_AudioSpec& spec = device->GetSpec();
        std::stringstream specTable;
        specTable << std::format("{:>12} : {:>8}", "Format", rad::AudioFormatToString(spec.format))
                  << std::endl;
        specTable << std::format("{:>12} : {:>8}", "Channels", spec.channels) << std::endl;
        specTable << std::format("{:>12} : {:>8}", "Frequency", spec.freq) << std::endl;
        specTable << std::format("{:>12} : {:>8}", "SampleFrames", device->GetSampleFrames())
                  << std::endl;
        RAD_LOG_MULTIMEDIA(info, "Audio Playback#{}: {}\n{}", i, device->GetName(),
                           specTable.str());
    }
    auto recordingDevices = audioSystem.GetAudioRecordingDevices();
    for (size_t i = 0; i < recordingDevices.size(); ++i)
    {
        auto& device = recordingDevices[i];
        const SDL_AudioSpec& spec = device->GetSpec();
        std::stringstream specTable;
        specTable << std::format("{:>12} : {:>8}", "Format", rad::AudioFormatToString(spec.format))
                  << std::endl;
        specTable << std::format("{:>12} : {:>8}", "Channels", spec.channels) << std::endl;
        specTable << std::format("{:>12} : {:>8}", "Frequency", spec.freq) << std::endl;
        specTable << std::format("{:>12} : {:>8}", "SampleFrames", device->GetSampleFrames())
                  << std::endl;
        RAD_LOG_MULTIMEDIA(info, "Audio Recording#{}: {}\n{}", i, device->GetName(),
                           specTable.str());
    }
    audioSystem.Shutdown();
}
