#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/GuiRenderer.h>

#include <rad/Gui/GuiTexture.h>

#include <SDL3/SDL_version.h>

#include <stdexcept>

namespace rad
{

GuiRenderer::GuiRenderer(Window* window, SDL_PropertiesID props) :
    m_window(window)
{
    if (!Init(props))
    {
        Destroy();
        throw std::runtime_error("Failed to create GuiRenderer");
    }
}

GuiRenderer::~GuiRenderer()
{
    Destroy();
}

int GuiRenderer::GetNumRenderDrivers()
{
    return SDL_GetNumRenderDrivers();
}

const char* GuiRenderer::GetRenderDriver(int index)
{
    return SDL_GetRenderDriver(index);
}

bool GuiRenderer::Init(SDL_PropertiesID props)
{
    assert(m_handle == nullptr);
    if (props == 0)
    {
        const char* driver = nullptr;

#if defined(RAD_OS_WINDOWS)
        for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
        {
            const char* name = SDL_GetRenderDriver(i);
            if (name && StrEqual(name, "direct3d12"))
            {
                driver = name;
                break;
            }
        }
        if (!driver)
        {
            for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i)
            {
                const char* name = SDL_GetRenderDriver(i);
                if (name && StrEqual(name, "direct3d11"))
                {
                    driver = name;
                    break;
                }
            }
        }
#endif
        m_handle = SDL_CreateRenderer(m_window->GetHandle(), driver);
        if (!m_handle)
        {
            RAD_LOG_GUI(err, "SDL_CreateRenderer failed: {}", SDL_GetError());
            return false;
        }
        if (const char* name = SDL_GetRendererName(m_handle))
        {
            m_name = name;
        }
        RAD_LOG_GUI(info, "GuiRenderer created: {}", m_name);
        if (SDL_GetRendererProperties(m_handle) == 0)
        {
            RAD_LOG_GUI(err, "SDL_GetRendererProperties failed: {}", SDL_GetError());
        }
    }
    else
    {
        if (!SDL_SetPointerProperty(props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER,
                                    m_window->GetHandle()))
        {
            RAD_LOG_GUI(
                err, "SDL_SetPointerProperty(SDL_PROP_RENDERER_CREATE_WINDOW_POINTER) failed: {}",
                SDL_GetError());
            return false;
        }
        m_handle = SDL_CreateRendererWithProperties(props);
        if (!m_handle)
        {
            RAD_LOG_GUI(err, "SDL_CreateRendererWithProperties failed: {}", SDL_GetError());
            return false;
        }
        if (const char* name = SDL_GetRendererName(m_handle))
        {
            m_name = name;
        }
        RAD_LOG_GUI(info, "GuiRenderer created: {}", m_name);
        if (SDL_GetRendererProperties(m_handle) == 0)
        {
            RAD_LOG_GUI(err, "SDL_GetRendererProperties failed: {}", SDL_GetError());
        }
    }
    return InitImGui();
}

bool GuiRenderer::InitImGui()
{
    assert(m_window && m_handle);
    IMGUI_CHECKVERSION();
    m_imgui = ImGui::CreateContext();
    if (!m_imgui)
    {
        RAD_LOG_GUI(err, "ImGui::CreateContext failed!");
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    float windowScale = m_window->GetDisplayScale();
    if (windowScale > 0)
    {
        style.ScaleAllSizes(windowScale);
        style.FontScaleDpi = windowScale * 1.2f;
    }

    if (!ImGui_ImplSDL3_InitForSDLRenderer(m_window->GetHandle(), m_handle))
    {
        RAD_LOG_GUI(err, "ImGui_ImplSDL3_InitForSDLRenderer failed!");
        return false;
    }
    if (!ImGui_ImplSDLRenderer3_Init(m_handle))
    {
        RAD_LOG_GUI(err, "ImGui_ImplSDLRenderer3_Init failed!");
        return false;
    }
    return true;
}

void GuiRenderer::Destroy()
{
    if (m_imgui)
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
        m_imgui = nullptr;
    }
    if (m_handle)
    {
        RAD_SDL_CHECK_GUI(SDL_SetRenderTarget(m_handle, nullptr));
        SDL_DestroyRenderer(m_handle);
        m_handle = nullptr;
    }
    m_name.clear();
}

SDL_PropertiesID GuiRenderer::GetRendererProperties() const
{
    return SDL_GetRendererProperties(m_handle);
}

SDL_Colorspace GuiRenderer::GetOutputColorspace() const
{
    const SDL_PropertiesID props = GetRendererProperties();
    if (props == 0)
    {
        return (SDL_Colorspace)0;
    }
    return (SDL_Colorspace)SDL_GetNumberProperty(props, SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER,
                                                 (Sint64)SDL_COLORSPACE_SRGB);
}

bool GuiRenderer::IsHDRDisplayEnabled() const
{
    const SDL_PropertiesID props = GetRendererProperties();
    if (props == 0)
    {
        return false;
    }
    return SDL_GetBooleanProperty(props, SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN, false);
}

float GuiRenderer::GetRendererSDRWhitePoint(float defaultValue) const
{
    const SDL_PropertiesID props = GetRendererProperties();
    if (props == 0)
    {
        return defaultValue;
    }
    return SDL_GetFloatProperty(props, SDL_PROP_RENDERER_SDR_WHITE_POINT_FLOAT, defaultValue);
}

float GuiRenderer::GetRendererHDRHeadroom(float defaultValue) const
{
    const SDL_PropertiesID props = GetRendererProperties();
    if (props == 0)
    {
        return defaultValue;
    }
    return SDL_GetFloatProperty(props, SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT, defaultValue);
}

int GuiRenderer::GetMaxTextureSize() const
{
    const SDL_PropertiesID props = GetRendererProperties();
    if (props == 0)
    {
        return 0;
    }
    return (int)SDL_GetNumberProperty(props, SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER, 0);
}

bool GuiRenderer::SupportsTextureWrappingNonPowerOfTwo() const
{
    const SDL_PropertiesID props = GetRendererProperties();
    if (props == 0)
    {
        return false;
    }
    return SDL_GetBooleanProperty(props, SDL_PROP_RENDERER_TEXTURE_WRAPPING_BOOLEAN, false);
}

const SDL_PixelFormat* GuiRenderer::GetTextureFormats() const
{
    const SDL_PropertiesID props = GetRendererProperties();
    if (props == 0)
    {
        return nullptr;
    }
    return static_cast<const SDL_PixelFormat*>(
        SDL_GetPointerProperty(props, SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER, nullptr));
}

SDL_Window* GuiRenderer::GetRenderWindowHandle() const
{
    return SDL_GetRenderWindow(m_handle);
}

bool GuiRenderer::GetOutputSize(int* w, int* h)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderOutputSize(m_handle, w, h));
}

bool GuiRenderer::GetCurrentOutputSize(int* w, int* h)
{
    return RAD_SDL_CHECK_GUI(SDL_GetCurrentRenderOutputSize(m_handle, w, h));
}

bool GuiRenderer::SetRenderTarget(GuiTexture* texture)
{
    return RAD_SDL_CHECK_GUI(
        SDL_SetRenderTarget(m_handle, texture ? texture->GetHandle() : nullptr));
}

SDL_Texture* GuiRenderer::GetRenderTarget() const
{
    return SDL_GetRenderTarget(m_handle);
}

bool GuiRenderer::SetLogicalPresentation(int w, int h, SDL_RendererLogicalPresentation mode)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderLogicalPresentation(m_handle, w, h, mode));
}

bool GuiRenderer::GetLogicalPresentation(int* w, int* h, SDL_RendererLogicalPresentation* mode)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderLogicalPresentation(m_handle, w, h, mode));
}

bool GuiRenderer::GetLogicalPresentationRect(SDL_FRect* rect)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderLogicalPresentationRect(m_handle, rect));
}

bool GuiRenderer::TransformWindowCoordToRender(float windowX, float windowY, float* x, float* y)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderCoordinatesFromWindow(m_handle, windowX, windowY, x, y));
}

bool GuiRenderer::TransformRenderCoordToWindow(float x, float y, float* windowX, float* windowY)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderCoordinatesToWindow(m_handle, x, y, windowX, windowY));
}

bool GuiRenderer::TransformCoordToRender(SDL_Event& event)
{
    return RAD_SDL_CHECK_GUI(SDL_ConvertEventToRenderCoordinates(m_handle, &event));
}

bool GuiRenderer::SetViewport(const SDL_Rect* rect)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderViewport(m_handle, rect));
}

bool GuiRenderer::SetViewportFull()
{
    return SetViewport(nullptr);
}

bool GuiRenderer::GetViewport(SDL_Rect* rect)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderViewport(m_handle, rect));
}

bool GuiRenderer::HasViewportSet()
{
    return SDL_RenderViewportSet(m_handle);
}

bool GuiRenderer::GetSafeArea(SDL_Rect* rect)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderSafeArea(m_handle, rect));
}

bool GuiRenderer::SetClipRect(const SDL_Rect* rect)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderClipRect(m_handle, rect));
}

bool GuiRenderer::DisableClipping()
{
    return SetClipRect(nullptr);
}

bool GuiRenderer::GetClipRect(SDL_Rect* rect)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderClipRect(m_handle, rect));
}

bool GuiRenderer::IsClipEnabled()
{
    return SDL_RenderClipEnabled(m_handle);
}

bool GuiRenderer::SetRenderScale(float scaleX, float scaleY)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderScale(m_handle, scaleX, scaleY));
}

bool GuiRenderer::GetRenderScale(float* scaleX, float* scaleY)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderScale(m_handle, scaleX, scaleY));
}

bool GuiRenderer::SetRenderDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderDrawColor(m_handle, r, g, b, a));
}

bool GuiRenderer::SetRenderDrawColor(float r, float g, float b, float a)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderDrawColorFloat(m_handle, r, g, b, a));
}

bool GuiRenderer::GetRenderDrawColor(Uint8* r, Uint8* g, Uint8* b, Uint8* a)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderDrawColor(m_handle, r, g, b, a));
}

bool GuiRenderer::GetRenderDrawColor(float* r, float* g, float* b, float* a)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderDrawColorFloat(m_handle, r, g, b, a));
}

bool GuiRenderer::SetColorScale(float scale)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderColorScale(m_handle, scale));
}

bool GuiRenderer::GetColorScale(float* scale)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderColorScale(m_handle, scale));
}

bool GuiRenderer::SetBlendMode(SDL_BlendMode blendMode)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderDrawBlendMode(m_handle, blendMode));
}

bool GuiRenderer::GetBlendMode(SDL_BlendMode* blendMode)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderDrawBlendMode(m_handle, blendMode));
}

bool GuiRenderer::Clear()
{
    return RAD_SDL_CHECK_GUI(SDL_RenderClear(m_handle));
}

bool GuiRenderer::DrawPoint(float x, float y)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderPoint(m_handle, x, y));
}

bool GuiRenderer::DrawPoints(const SDL_FPoint* points, int count)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderPoints(m_handle, points, count));
}

bool GuiRenderer::DrawLine(float x1, float y1, float x2, float y2)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderLine(m_handle, x1, y1, x2, y2));
}

bool GuiRenderer::DrawLines(const SDL_FPoint* points, int count)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderLines(m_handle, points, count));
}

bool GuiRenderer::DrawRect(const SDL_FRect* rect)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderRect(m_handle, rect));
}

bool GuiRenderer::DrawRects(const SDL_FRect* rects, int count)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderRects(m_handle, rects, count));
}

bool GuiRenderer::FillRect(const SDL_FRect* rect)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderFillRect(m_handle, rect));
}

bool GuiRenderer::FillRects(const SDL_FRect* rects, int count)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderFillRects(m_handle, rects, count));
}

bool GuiRenderer::DrawTexture(GuiTexture* texture, const SDL_FRect* srcRect,
                              const SDL_FRect* dstRect)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderTexture(m_handle, texture->GetHandle(), srcRect, dstRect));
}

bool GuiRenderer::DrawTextureRotated(GuiTexture* texture, const SDL_FRect* srcRect,
                                     const SDL_FRect* dstRect, const double angle,
                                     const SDL_FPoint* center, const SDL_FlipMode flip)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderTextureRotated(m_handle, texture->GetHandle(), srcRect,
                                                      dstRect, angle, center, flip));
}

bool GuiRenderer::DrawTextureAffine(GuiTexture* texture, const SDL_FRect* srcRect,
                                    const SDL_FPoint* origin, const SDL_FPoint* right,
                                    const SDL_FPoint* down)
{
    return RAD_SDL_CHECK_GUI(
        SDL_RenderTextureAffine(m_handle, texture->GetHandle(), srcRect, origin, right, down));
}

bool GuiRenderer::DrawTextureTiled(GuiTexture* texture, const SDL_FRect* srcRect, float scale,
                                   const SDL_FRect* dstRect)
{
    return RAD_SDL_CHECK_GUI(
        SDL_RenderTextureTiled(m_handle, texture->GetHandle(), srcRect, scale, dstRect));
}

bool GuiRenderer::DrawTexture9Grid(GuiTexture* texture, const SDL_FRect* srcRect, float leftWidth,
                                   float rightWidth, float topHeight, float bottomHeight,
                                   float scale, const SDL_FRect* dstRect)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderTexture9Grid(m_handle, texture->GetHandle(), srcRect,
                                                    leftWidth, rightWidth, topHeight, bottomHeight,
                                                    scale, dstRect));
}

bool GuiRenderer::DrawTexture9GridTiled(GuiTexture* texture, const SDL_FRect* srcRect,
                                        float leftWidth, float rightWidth, float topHeight,
                                        float bottomHeight, float scale, const SDL_FRect* dstRect,
                                        float tileScale)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderTexture9GridTiled(m_handle, texture->GetHandle(), srcRect,
                                                         leftWidth, rightWidth, topHeight,
                                                         bottomHeight, scale, dstRect, tileScale));
}

bool GuiRenderer::RenderGeometry(GuiTexture* texture, const SDL_Vertex* vertices, int numVertices,
                                 const int* indices, int numIndices)
{
    SDL_Texture* const tex = texture ? texture->GetHandle() : nullptr;
    return RAD_SDL_CHECK_GUI(
        SDL_RenderGeometry(m_handle, tex, vertices, numVertices, indices, numIndices));
}

bool GuiRenderer::RenderGeometryRaw(GuiTexture* texture, const float* xy, int xyStride,
                                    const SDL_FColor* color, int colorStride, const float* uv,
                                    int uvStride, int numVertices, const void* indices,
                                    int numIndices, int indexType)
{
    SDL_Texture* const tex = texture ? texture->GetHandle() : nullptr;
    return RAD_SDL_CHECK_GUI(SDL_RenderGeometryRaw(m_handle, tex, xy, xyStride, color, colorStride,
                                                   uv, uvStride, numVertices, indices, numIndices,
                                                   indexType));
}

bool GuiRenderer::SetTextureAddressMode(SDL_TextureAddressMode uMode, SDL_TextureAddressMode vMode)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderTextureAddressMode(m_handle, uMode, vMode));
}

bool GuiRenderer::GetTextureAddressMode(SDL_TextureAddressMode* uMode,
                                        SDL_TextureAddressMode* vMode)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderTextureAddressMode(m_handle, uMode, vMode));
}

bool GuiRenderer::SetDefaultTextureScaleMode(SDL_ScaleMode scaleMode)
{
    return RAD_SDL_CHECK_GUI(SDL_SetDefaultTextureScaleMode(m_handle, scaleMode));
}

bool GuiRenderer::GetDefaultTextureScaleMode(SDL_ScaleMode* scaleMode)
{
    return RAD_SDL_CHECK_GUI(SDL_GetDefaultTextureScaleMode(m_handle, scaleMode));
}

Ref<Surface> GuiRenderer::ReadPixels(const SDL_Rect* rect)
{
    if (SDL_Surface* pixels = SDL_RenderReadPixels(m_handle, rect))
    {
        return RAD_NEW Surface(pixels, true);
    }
    RAD_LOG_GUI(err, "SDL_RenderReadPixels failed: {}", SDL_GetError());
    return nullptr;
}

bool GuiRenderer::Present()
{
    return RAD_SDL_CHECK_GUI(SDL_RenderPresent(m_handle));
}

bool GuiRenderer::Flush()
{
    return RAD_SDL_CHECK_GUI(SDL_FlushRenderer(m_handle));
}

bool GuiRenderer::SetVSync(int vsync)
{
    return RAD_SDL_CHECK_GUI(SDL_SetRenderVSync(m_handle, vsync));
}

bool GuiRenderer::GetVSync(int* vsync)
{
    return RAD_SDL_CHECK_GUI(SDL_GetRenderVSync(m_handle, vsync));
}

bool GuiRenderer::DrawDebugText(float x, float y, const char* text)
{
    return RAD_SDL_CHECK_GUI(SDL_RenderDebugText(m_handle, x, y, text));
}

void* GuiRenderer::GetMetalLayer() const
{
    return SDL_GetRenderMetalLayer(m_handle);
}

void* GuiRenderer::GetMetalCommandEncoder() const
{
    return SDL_GetRenderMetalCommandEncoder(m_handle);
}

bool GuiRenderer::AddVulkanRenderSemaphores(Uint32 waitStageMask, Sint64 waitSemaphore,
                                            Sint64 signalSemaphore)
{
    return RAD_SDL_CHECK_GUI(
        SDL_AddVulkanRenderSemaphores(m_handle, waitStageMask, waitSemaphore, signalSemaphore));
}

#if SDL_VERSION_ATLEAST(3, 4, 0)

SDL_GPUDevice* GuiRenderer::GetGPUDevice() const
{
    return SDL_GetGPURendererDevice(m_handle);
}

SDL_GPURenderState* GuiRenderer::CreateGPURenderState(
    const SDL_GPURenderStateCreateInfo* createInfo)
{
    if (SDL_GPURenderState* state = SDL_CreateGPURenderState(m_handle, createInfo))
    {
        return state;
    }
    RAD_LOG_GUI(err, "SDL_CreateGPURenderState failed: {}", SDL_GetError());
    return nullptr;
}

bool GuiRenderer::SetGPURenderState(SDL_GPURenderState* state)
{
    return RAD_SDL_CHECK_GUI(SDL_SetGPURenderState(m_handle, state));
}

void GuiRenderer::DestroyGPURenderState(SDL_GPURenderState* state)
{
    SDL_DestroyGPURenderState(state);
}

bool GuiRenderer::SetGPURenderStateFragmentUniforms(SDL_GPURenderState* state, Uint32 slotIndex,
                                                    const void* data, Uint32 length)
{
    return RAD_SDL_CHECK_GUI(SDL_SetGPURenderStateFragmentUniforms(state, slotIndex, data, length));
}

#endif

#if SDL_VERSION_ATLEAST(3, 6, 0)

bool GuiRenderer::SetGPURenderStateSamplerBindings(
    SDL_GPURenderState* state, int numSamplerBindings,
    const SDL_GPUTextureSamplerBinding* samplerBindings)
{
    return RAD_SDL_CHECK_GUI(
        SDL_SetGPURenderStateSamplerBindings(state, numSamplerBindings, samplerBindings));
}

bool GuiRenderer::SetGPURenderStateStorageTextures(SDL_GPURenderState* state,
                                                   int numStorageTextures,
                                                   SDL_GPUTexture* const* storageTextures)
{
    return RAD_SDL_CHECK_GUI(
        SDL_SetGPURenderStateStorageTextures(state, numStorageTextures, storageTextures));
}

bool GuiRenderer::SetGPURenderStateStorageBuffers(SDL_GPURenderState* state, int numStorageBuffers,
                                                  SDL_GPUBuffer* const* storageBuffers)
{
    return RAD_SDL_CHECK_GUI(
        SDL_SetGPURenderStateStorageBuffers(state, numStorageBuffers, storageBuffers));
}

#endif

#ifdef SDL_PLATFORM_GDK
void GuiRenderer::GDKSuspendRenderer()
{
    if (m_handle)
    {
        SDL_GDKSuspendRenderer(m_handle);
    }
}

void GuiRenderer::GDKResumeRenderer()
{
    if (m_handle)
    {
        SDL_GDKResumeRenderer(m_handle);
    }
}
#endif

void GuiRenderer::BeginFrame()
{
    ImGui_ImplSDL3_NewFrame();
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui::NewFrame();
}

void GuiRenderer::EndFrame()
{
    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    // Match ImGui HiDPI framebuffer scale; avoid mixing with other SDL_SetRenderScale callers.
    SDL_SetRenderScale(m_handle, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    Clear();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_handle);
    Present();
}

bool GuiRenderer::ProcessEvent(const SDL_Event& event)
{
    return ImGui_ImplSDL3_ProcessEvent(&event);
}

} // namespace rad
