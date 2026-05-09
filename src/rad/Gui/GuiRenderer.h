#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/Surface.h>
#include <rad/Gui/Window.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_version.h>

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

#include <format>
#include <string>

namespace rad
{

class GuiTexture;

// 2D rendering context for a window. The associated Window must outlive the GuiRenderer.
// Construction creates the SDL renderer and initializes ImGui backends; throws std::runtime_error on failure.
// SDL render APIs must be used on the main thread; see SDL issue #986.
// https://wiki.libsdl.org/SDL3/CategoryRender
// https://github.com/libsdl-org/SDL/issues/986
class GuiRenderer : public RefCounted<GuiRenderer>
{
public:
    // SDL_CreateRenderer or SDL_CreateRendererWithProperties (when props != 0), then ImGui backends.
    // https://wiki.libsdl.org/SDL3/SDL_CreateRenderer
    // https://wiki.libsdl.org/SDL3/SDL_CreateRendererWithProperties
    GuiRenderer(Window* window, SDL_PropertiesID props = 0);
    // Calls Destroy().
    // https://wiki.libsdl.org/SDL3/SDL_DestroyRenderer
    ~GuiRenderer();

    // https://wiki.libsdl.org/SDL3/SDL_GetNumRenderDrivers
    static int GetNumRenderDrivers();
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderDriver
    static const char* GetRenderDriver(int index);

    // Returns m_handle.
    SDL_Renderer* GetHandle() const { return m_handle; }
    // https://wiki.libsdl.org/SDL3/SDL_GetRendererProperties
    SDL_PropertiesID GetRendererProperties() const;

    // SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER via SDL_GetNumberProperty on renderer properties.
    // https://wiki.libsdl.org/SDL3/SDL_GetRendererProperties
    SDL_Colorspace GetOutputColorspace() const;
    // SDL_PROP_RENDERER_HDR_ENABLED_BOOLEAN via SDL_GetBooleanProperty.
    // https://wiki.libsdl.org/SDL3/SDL_GetRendererProperties
    bool IsHDRDisplayEnabled() const;
    // SDL_PROP_RENDERER_SDR_WHITE_POINT_FLOAT via SDL_GetFloatProperty.
    // https://wiki.libsdl.org/SDL3/SDL_GetFloatProperty
    float GetRendererSDRWhitePoint(float defaultValue) const;
    // SDL_PROP_RENDERER_HDR_HEADROOM_FLOAT via SDL_GetFloatProperty.
    // https://wiki.libsdl.org/SDL3/SDL_GetFloatProperty
    float GetRendererHDRHeadroom(float defaultValue) const;
    // SDL_PROP_RENDERER_MAX_TEXTURE_SIZE_NUMBER via SDL_GetNumberProperty.
    // https://wiki.libsdl.org/SDL3/SDL_GetRendererProperties
    int GetMaxTextureSize() const;
    // SDL_PROP_RENDERER_TEXTURE_WRAPPING_BOOLEAN (wrap address mode on non-power-of-two textures).
    // https://wiki.libsdl.org/SDL3/SDL_GetRendererProperties
    bool SupportsTextureWrappingNonPowerOfTwo() const;
    // SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER via SDL_GetPointerProperty; list ends with SDL_PIXELFORMAT_UNKNOWN.
    // https://wiki.libsdl.org/SDL3/SDL_GetPointerProperty
    const SDL_PixelFormat* GetTextureFormats() const;

    // Returns m_window.
    Window* GetWindow() { return m_window; }
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderWindow
    SDL_Window* GetRenderWindowHandle() const;
    // Set from SDL_GetRendererName when the SDL renderer is created (stable for the renderer lifetime).
    // https://wiki.libsdl.org/SDL3/SDL_GetRendererName
    const std::string& GetName() const { return m_name; }

    // https://wiki.libsdl.org/SDL3/SDL_GetRenderOutputSize
    bool GetOutputSize(int* w, int* h);
    // https://wiki.libsdl.org/SDL3/SDL_GetCurrentRenderOutputSize
    bool GetCurrentOutputSize(int* w, int* h);

    // https://wiki.libsdl.org/SDL3/SDL_SetRenderTarget
    bool SetRenderTarget(GuiTexture* texture);
    // Same as SetRenderTarget(nullptr).
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderTarget
    bool SetRenderToWindow() { return SetRenderTarget(nullptr); }
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderTarget
    SDL_Texture* GetRenderTarget() const;

    // https://wiki.libsdl.org/SDL3/SDL_SetRenderLogicalPresentation
    bool SetLogicalPresentation(int w, int h, SDL_RendererLogicalPresentation mode);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderLogicalPresentation
    bool GetLogicalPresentation(int* w, int* h, SDL_RendererLogicalPresentation* mode);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderLogicalPresentationRect
    bool GetLogicalPresentationRect(SDL_FRect* rect);
    // Delegates to GetLogicalPresentationRect(SDL_FRect*).
    bool GetLogicalPresentationRect(SDL_FRect& rect) { return GetLogicalPresentationRect(&rect); }

    // https://wiki.libsdl.org/SDL3/SDL_RenderCoordinatesFromWindow
    bool TransformWindowCoordToRender(float windowX, float windowY, float* x, float* y);
    // https://wiki.libsdl.org/SDL3/SDL_RenderCoordinatesToWindow
    bool TransformRenderCoordToWindow(float x, float y, float* windowX, float* windowY);
    // https://wiki.libsdl.org/SDL3/SDL_ConvertEventToRenderCoordinates
    bool TransformCoordToRender(SDL_Event& event);

    // https://wiki.libsdl.org/SDL3/SDL_SetRenderViewport
    bool SetViewport(const SDL_Rect* rect);
    // Same as SetViewport(nullptr).
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderViewport
    bool SetViewportFull();
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderViewport
    bool GetViewport(SDL_Rect* rect);
    // Delegates to SetViewport(const SDL_Rect*).
    bool SetViewport(const SDL_Rect& rect) { return SetViewport(&rect); }
    // Delegates to GetViewport(SDL_Rect*).
    bool GetViewport(SDL_Rect& rect) { return GetViewport(&rect); }
    // https://wiki.libsdl.org/SDL3/SDL_RenderViewportSet
    bool HasViewportSet();
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderSafeArea
    bool GetSafeArea(SDL_Rect* rect);
    // Delegates to GetSafeArea(SDL_Rect*).
    bool GetSafeArea(SDL_Rect& rect) { return GetSafeArea(&rect); }
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderClipRect
    bool SetClipRect(const SDL_Rect* rect);
    // Same as SetClipRect(nullptr).
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderClipRect
    bool DisableClipping();
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderClipRect
    bool GetClipRect(SDL_Rect* rect);
    // Delegates to SetClipRect(const SDL_Rect*).
    bool SetClipRect(const SDL_Rect& rect) { return SetClipRect(&rect); }
    // Delegates to GetClipRect(SDL_Rect*).
    bool GetClipRect(SDL_Rect& rect) { return GetClipRect(&rect); }
    // https://wiki.libsdl.org/SDL3/SDL_RenderClipEnabled
    bool IsClipEnabled();
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderScale
    bool SetRenderScale(float scaleX, float scaleY);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderScale
    bool GetRenderScale(float* scaleX, float* scaleY);

    // https://wiki.libsdl.org/SDL3/SDL_SetRenderDrawColor
    bool SetRenderDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderDrawColorFloat
    bool SetRenderDrawColor(float r, float g, float b, float a);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderDrawColor
    bool GetRenderDrawColor(Uint8* r, Uint8* g, Uint8* b, Uint8* a);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderDrawColorFloat
    bool GetRenderDrawColor(float* r, float* g, float* b, float* a);

    // https://wiki.libsdl.org/SDL3/SDL_SetRenderColorScale
    bool SetColorScale(float scale);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderColorScale
    bool GetColorScale(float* scale);
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderDrawBlendMode
    bool SetBlendMode(SDL_BlendMode blendMode);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderDrawBlendMode
    bool GetBlendMode(SDL_BlendMode* blendMode);

    // https://wiki.libsdl.org/SDL3/SDL_RenderClear
    bool Clear();

    // https://wiki.libsdl.org/SDL3/SDL_RenderPoint
    bool DrawPoint(float x, float y);
    // https://wiki.libsdl.org/SDL3/SDL_RenderPoints
    bool DrawPoints(const SDL_FPoint* points, int count);
    // https://wiki.libsdl.org/SDL3/SDL_RenderLine
    bool DrawLine(float x1, float y1, float x2, float y2);
    // https://wiki.libsdl.org/SDL3/SDL_RenderLines
    bool DrawLines(const SDL_FPoint* points, int count);
    // https://wiki.libsdl.org/SDL3/SDL_RenderRect
    bool DrawRect(const SDL_FRect* rect);
    // https://wiki.libsdl.org/SDL3/SDL_RenderRects
    bool DrawRects(const SDL_FRect* rects, int count);
    // https://wiki.libsdl.org/SDL3/SDL_RenderFillRect
    bool FillRect(const SDL_FRect* rect);
    // https://wiki.libsdl.org/SDL3/SDL_RenderFillRects
    bool FillRects(const SDL_FRect* rects, int count);

    // https://wiki.libsdl.org/SDL3/SDL_RenderTexture
    bool DrawTexture(GuiTexture* texture, const SDL_FRect* srcRect, const SDL_FRect* dstRect);
    // https://wiki.libsdl.org/SDL3/SDL_RenderTextureRotated
    bool DrawTextureRotated(GuiTexture* texture, const SDL_FRect* srcRect, const SDL_FRect* dstRect,
                            const double angle, const SDL_FPoint* center, const SDL_FlipMode flip);
    // https://wiki.libsdl.org/SDL3/SDL_RenderTextureAffine
    bool DrawTextureAffine(GuiTexture* texture, const SDL_FRect* srcRect, const SDL_FPoint* origin,
                           const SDL_FPoint* right, const SDL_FPoint* down);
    // https://wiki.libsdl.org/SDL3/SDL_RenderTextureTiled
    bool DrawTextureTiled(GuiTexture* texture, const SDL_FRect* srcRect, float scale,
                          const SDL_FRect* dstRect);
    // https://wiki.libsdl.org/SDL3/SDL_RenderTexture9Grid
    bool DrawTexture9Grid(GuiTexture* texture, const SDL_FRect* srcRect, float leftWidth,
                          float rightWidth, float topHeight, float bottomHeight, float scale,
                          const SDL_FRect* dstRect);
    // https://wiki.libsdl.org/SDL3/SDL_RenderTexture9GridTiled
    bool DrawTexture9GridTiled(GuiTexture* texture, const SDL_FRect* srcRect, float leftWidth,
                               float rightWidth, float topHeight, float bottomHeight, float scale,
                               const SDL_FRect* dstRect, float tileScale);
    // Texture may be nullptr (vertex colors only).
    // https://wiki.libsdl.org/SDL3/SDL_RenderGeometry
    bool RenderGeometry(GuiTexture* texture, const SDL_Vertex* vertices, int numVertices,
                        const int* indices, int numIndices);
    // https://wiki.libsdl.org/SDL3/SDL_RenderGeometryRaw
    bool RenderGeometryRaw(GuiTexture* texture, const float* xy, int xyStride,
                           const SDL_FColor* color, int colorStride, const float* uv, int uvStride,
                           int numVertices, const void* indices, int numIndices, int indexType);
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderTextureAddressMode
    bool SetTextureAddressMode(SDL_TextureAddressMode uMode, SDL_TextureAddressMode vMode);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderTextureAddressMode
    bool GetTextureAddressMode(SDL_TextureAddressMode* uMode, SDL_TextureAddressMode* vMode);
    // https://wiki.libsdl.org/SDL3/SDL_SetDefaultTextureScaleMode
    bool SetDefaultTextureScaleMode(SDL_ScaleMode scaleMode);
    // https://wiki.libsdl.org/SDL3/SDL_GetDefaultTextureScaleMode
    bool GetDefaultTextureScaleMode(SDL_ScaleMode* scaleMode);

    // https://wiki.libsdl.org/SDL3/SDL_RenderReadPixels
    Ref<Surface> ReadPixels(const SDL_Rect* rect);
    // Delegates to ReadPixels(const SDL_Rect*).
    Ref<Surface> ReadPixels(const SDL_Rect& rect) { return ReadPixels(&rect); }

    // https://wiki.libsdl.org/SDL3/SDL_RenderPresent
    bool Present();
    // https://wiki.libsdl.org/SDL3/SDL_FlushRenderer
    bool Flush();

    // https://wiki.libsdl.org/SDL3/SDL_SetRenderVSync
    bool SetVSync(int vsync);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderVSync
    bool GetVSync(int* vsync);

    // 8x8 px glyphs; SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE.
    // https://wiki.libsdl.org/SDL3/SDL_RenderDebugText
    bool DrawDebugText(float x, float y, const char* text);
    // std::format, then DrawDebugText.
    template <typename... Args>
    bool DrawDebugTextFormat(float x, float y, std::format_string<Args...> fmt, Args&&... args)
    {
        const std::string s = std::format(fmt, std::forward<Args>(args)...);
        return DrawDebugText(x, y, s.c_str());
    }

    // https://wiki.libsdl.org/SDL3/SDL_GetRenderMetalLayer
    void* GetMetalLayer() const;
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderMetalCommandEncoder
    void* GetMetalCommandEncoder() const;
    // https://wiki.libsdl.org/SDL3/SDL_AddVulkanRenderSemaphores
    bool AddVulkanRenderSemaphores(Uint32 waitStageMask, Sint64 waitSemaphore,
                                   Sint64 signalSemaphore);

#if SDL_VERSION_ATLEAST(3, 4, 0)
    // nullptr when the renderer is not GPU-backed.
    // https://wiki.libsdl.org/SDL3/SDL_GetGPURendererDevice
    SDL_GPUDevice* GetGPUDevice() const;
    // https://wiki.libsdl.org/SDL3/SDL_CreateGPURenderState
    SDL_GPURenderState* CreateGPURenderState(const SDL_GPURenderStateCreateInfo* createInfo);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPURenderState
    bool SetGPURenderState(SDL_GPURenderState* state);
    // https://wiki.libsdl.org/SDL3/SDL_DestroyGPURenderState
    void DestroyGPURenderState(SDL_GPURenderState* state);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPURenderStateFragmentUniforms
    static bool SetGPURenderStateFragmentUniforms(SDL_GPURenderState* state, Uint32 slotIndex,
                                                  const void* data, Uint32 length);
#endif
#if SDL_VERSION_ATLEAST(3, 6, 0)
    // https://wiki.libsdl.org/SDL3/SDL_SetGPURenderStateSamplerBindings
    static bool SetGPURenderStateSamplerBindings(
        SDL_GPURenderState* state, int numSamplerBindings,
        const SDL_GPUTextureSamplerBinding* samplerBindings);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPURenderStateStorageTextures
    static bool SetGPURenderStateStorageTextures(SDL_GPURenderState* state, int numStorageTextures,
                                                 SDL_GPUTexture* const* storageTextures);
    // https://wiki.libsdl.org/SDL3/SDL_SetGPURenderStateStorageBuffers
    static bool SetGPURenderStateStorageBuffers(SDL_GPURenderState* state, int numStorageBuffers,
                                                SDL_GPUBuffer* const* storageBuffers);
#endif

#ifdef SDL_PLATFORM_GDK
    // https://wiki.libsdl.org/SDL3/SDL_GDKSuspendRenderer
    void GDKSuspendRenderer();
    // https://wiki.libsdl.org/SDL3/SDL_GDKResumeRenderer
    void GDKResumeRenderer();
#endif

    // ImGui_ImplSDL3_NewFrame / ImGui_ImplSDLRenderer3_NewFrame / ImGui::NewFrame.
    void BeginFrame();
    // ImGui draw path: SDL_SetRenderScale, Clear (SDL_RenderClear), ImGui_ImplSDLRenderer3_RenderDrawData,
    // Present (SDL_RenderPresent).
    // https://wiki.libsdl.org/SDL3/CategoryRender
    void EndFrame();

    // ImGui_ImplSDL3_ProcessEvent.
    bool ProcessEvent(const SDL_Event& event);

private:
    // props == 0: SDL_CreateRenderer; else SDL_CreateRendererWithProperties (window pointer in props).
    // https://wiki.libsdl.org/SDL3/SDL_CreateRenderer
    // https://wiki.libsdl.org/SDL3/SDL_CreateRendererWithProperties
    bool Init(SDL_PropertiesID props = 0);
    // ImGui_ImplSDLRenderer3_Init and related Dear ImGui setup for this renderer.
    bool InitImGui();
    // Shuts down ImGui, clears the render target, and destroys the SDL renderer.
    // https://wiki.libsdl.org/SDL3/SDL_DestroyRenderer
    void Destroy();

    Window* m_window;
    SDL_Renderer* m_handle = nullptr;
    std::string m_name;

    ImGuiContext* m_imgui = nullptr;

}; // class GuiRenderer

} // namespace rad
