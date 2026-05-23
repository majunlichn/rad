#include <rad/ML/ML.test.h>

#include <rad/ML/MLBackend.h>
#include <rad/ML/MLCommon.h>
#include <rad/ML/MLDevice.h>
#include <rad/ML/MLGlobal.h>

#include <string_view>

using namespace rad;

TEST(ML, CpuBackendTest)
{
    MLBackend* backend = GetMLBackend("cpu");
    ASSERT_NE(backend, nullptr);
    EXPECT_EQ(backend->GetName(), std::string_view("cpu"));

    const size_t deviceCount = backend->GetDeviceCount();
    ASSERT_GT(deviceCount, 0u);

    for (size_t i = 0; i < deviceCount; ++i)
    {
        MLDevice* device = backend->GetDevice(i);
        ASSERT_NE(device, nullptr);
        RAD_LOG_ML(info, "CPU device#{}: {}", i, device->GetName());
    }
}
