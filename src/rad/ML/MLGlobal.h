#pragma once

#include <rad/Common/RefCounted.h>

#include <string_view>

namespace rad
{

class MLBackend;
class MLDevice;

void MLInit();
void MLShutdown();

void RegisterMLBackend(Ref<MLBackend> backend);
void UnregisterMLBackend(std::string_view name);
[[nodiscard]] MLBackend* GetMLBackend(std::string_view name);

// Device id syntax: "<backend>" or "<backend>:<index>" (e.g. "cpu", "cpu:0").
[[nodiscard]] MLDevice* GetMLDevice(std::string_view deviceId) noexcept;

} // namespace rad
