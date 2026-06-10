#include <rad/ML/ML.test.h>

#include <rad/ML/MLBackend.h>
#include <rad/ML/MLCommon.h>
#include <rad/ML/MLDevice.h>
#include <rad/ML/MLGlobal.h>

#include <array>
#include <string_view>

using namespace rad;

TEST(CpuML, Backend)
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

TEST(CpuML, DeviceDataTypeSupport)
{
    MLDevice* device = GetMLDevice("cpu");
    ASSERT_NE(device, nullptr);

    constexpr std::array supportedDataTypes = {
        MLDataType::Float32,  MLDataType::Float64,    MLDataType::Int8,       MLDataType::Int16,
        MLDataType::Int32,    MLDataType::Int64,      MLDataType::Uint8,      MLDataType::Uint16,
        MLDataType::Uint32,   MLDataType::Uint64,     MLDataType::Bool,       MLDataType::Float16,
        MLDataType::BFloat16, MLDataType::Float8E4M3, MLDataType::Float8E5M2,
    };

    for (const MLDataType dataType : supportedDataTypes)
    {
        EXPECT_TRUE(device->IsDataTypeSupported(dataType)) << GetDataTypeName(dataType);
    }

    EXPECT_FALSE(device->IsDataTypeSupported(MLDataType::Undefined));
}
