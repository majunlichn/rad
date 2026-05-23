#include <rad/ML/MLGlobal.h>

#include <rad/ML/MLBackend.h>
#include <rad/ML/MLDevice.h>
#include <rad/ML/Cpu/CpuMLBackend.h>

#include <rad/Common/String.h>

#include <charconv>
#include <format>
#include <map>
#include <stdexcept>
#include <string>

namespace rad
{

namespace
{

std::map<std::string, Ref<MLBackend>, StringLess> g_MLBackends;
bool g_MLInitialized = false;

[[nodiscard]] bool ParseDeviceIndex(std::string_view deviceIndexString, size_t& outIndex) noexcept
{
    if (deviceIndexString.empty())
    {
        return false;
    }

    size_t value = 0;
    const auto result = std::from_chars(deviceIndexString.data(),
                                        deviceIndexString.data() + deviceIndexString.size(), value);
    if (result.ec != std::errc{} ||
        result.ptr != deviceIndexString.data() + deviceIndexString.size())
    {
        return false;
    }

    outIndex = value;
    return true;
}

} // namespace

void MLInit()
{
    if (g_MLInitialized)
    {
        return;
    }

    RegisterMLBackend(Ref<CpuMLBackend>(new CpuMLBackend()));
    g_MLInitialized = true;
}

void MLShutdown()
{
    if (!g_MLInitialized)
    {
        return;
    }

    g_MLBackends.clear();
    g_MLInitialized = false;
}

void RegisterMLBackend(Ref<MLBackend> backend)
{
    if (!backend)
    {
        throw std::invalid_argument("RegisterMLBackend requires a valid backend");
    }

    const std::string_view name = backend->GetName();
    if (name.empty())
    {
        throw std::invalid_argument("RegisterMLBackend requires a non-empty backend name");
    }

    if (g_MLBackends.contains(name))
    {
        throw std::invalid_argument(std::format("ML backend '{}' is already registered", name));
    }

    g_MLBackends.emplace(std::string(name), std::move(backend));
}

void UnregisterMLBackend(std::string_view name)
{
    const auto it = g_MLBackends.find(name);
    if (it != g_MLBackends.end())
    {
        g_MLBackends.erase(it);
    }
}

MLBackend* GetMLBackend(std::string_view name)
{
    const auto it = g_MLBackends.find(name);
    if (it != g_MLBackends.end())
    {
        return it->second.get();
    }
    return nullptr;
}

MLDevice* GetMLDevice(std::string_view deviceId) noexcept
{
    if (!g_MLInitialized || deviceId.empty())
    {
        return nullptr;
    }

    std::string_view backendName = deviceId;
    size_t deviceIndex = 0;

    const size_t colon = deviceId.find(':');
    if (colon != std::string_view::npos)
    {
        if (colon == 0)
        {
            return nullptr;
        }

        backendName = deviceId.substr(0, colon);
        const std::string_view deviceIndexString = deviceId.substr(colon + 1);
        if (deviceIndexString.empty())
        {
            return nullptr;
        }

        if (!ParseDeviceIndex(deviceIndexString, deviceIndex))
        {
            return nullptr;
        }
    }

    MLBackend* backend = GetMLBackend(backendName);
    if (!backend)
    {
        return nullptr;
    }

    return backend->GetDevice(deviceIndex);
}

} // namespace rad
