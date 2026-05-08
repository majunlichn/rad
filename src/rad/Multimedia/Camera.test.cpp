#include <rad/Multimedia/Camera.h>

#include "Multimedia.test.h"

TEST(Camerma, EnumerateDrivers)
{
    auto& cameraSystem = rad::CameraSystem::GetInstance();
    ASSERT_TRUE(cameraSystem.Init());
    auto drivers = cameraSystem.GetCameraDrivers();
    for (size_t i = 0; i < drivers.size(); ++i)
    {
        RAD_LOG_MULTIMEDIA(info, "CameraDriver#{}: {}", i, drivers[i]);
    }
    if (const char* driver = cameraSystem.GetCurrentCameraDriver())
    {
        RAD_LOG_MULTIMEDIA(info, "Current CameraDriver: {}", driver);
    }
    cameraSystem.Shutdown();
}

TEST(Camerma, EnumerateDevices)
{
    auto& cameraSystem = rad::CameraSystem::GetInstance();
    ASSERT_TRUE(cameraSystem.Init());
    std::vector<rad::Ref<rad::Camera>> cameras = cameraSystem.GetCameras();
    for (size_t i = 0; i < cameras.size(); ++i)
    {
        RAD_LOG_MULTIMEDIA(info, "Camera#{}: {}", i, cameras[i]->GetName());
    }
    cameraSystem.Shutdown();
}
