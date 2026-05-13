#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/RHI/GpuDevice.h>

#include <SDL3/SDL_gpu.h>

namespace rad
{

// https://wiki.libsdl.org/SDL3/SDL_CreateGPUShader
class GpuShader : public RefCounted<GpuShader>
{
public:
    static Ref<GpuShader> Create(Ref<GpuDevice> device, const SDL_GPUShaderCreateInfo& createInfo);

    ~GpuShader();
    void Destroy();

    SDL_GPUShader* GetHandle() const { return m_handle; }

private:
    GpuShader(Ref<GpuDevice> device, SDL_GPUShader* handle);

    Ref<GpuDevice> m_device;
    SDL_GPUShader* m_handle = nullptr;

}; // class GpuShader

} // namespace rad
