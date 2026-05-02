#pragma once

#include <rad/Gui/GuiCommon.h>
#include <rad/Gui/Surface.h>
#include <rad/Gui/Window.h>

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_version.h>

#include <format>
#include <string>

namespace rad
{

class GuiTexture;

// 2D rendering context for a window. The associated Window must outlive the GuiRenderer.
// Should only be used on the main thread: https://github.com/libsdl-org/SDL/issues/986
class GuiRenderer : public RefCounted<GuiRenderer>
{
public:
    GuiRenderer(Ref<Window> window);
    ~GuiRenderer();

    static int GetNumRenderDrivers();
    static const char* GetRenderDriver(int index);

    bool Init();
    // Sets SDL_PROP_RENDERER_CREATE_WINDOW_POINTER to this object's window, then calls SDL_CreateRendererWithProperties.
    // https://wiki.libsdl.org/SDL3/SDL_CreateRendererWithProperties
    bool InitWithProperties(SDL_PropertiesID props);
    void Destroy();

    SDL_Renderer* GetHandle() const { return m_handle; }
    // https://wiki.libsdl.org/SDL3/SDL_GetRendererProperties
    SDL_PropertiesID GetRendererProperties() const;

    // Convenience accessors for fields documented on SDL_GetRendererProperties.
    SDL_Colorspace GetOutputColorspace() const;
    bool IsHDRDisplayEnabled() const;
    float GetRendererSDRWhitePoint(float defaultValue) const;
    float GetRendererHDRHeadroom(float defaultValue) const;
    int GetMaxTextureSize() const;
    // SDL_PROP_RENDERER_TEXTURE_WRAPPING_BOOLEAN (WRAP address mode on non-power-of-two textures).
    bool SupportsNPOTTextureWrap() const;
    // Supported texture pixel formats, ending with SDL_PIXELFORMAT_UNKNOWN; pointer valid for the renderer lifetime.
    const SDL_PixelFormat* GetTextureFormats() const;

    Window* GetWindow() { return m_window.get(); }
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderWindow
    SDL_Window* GetRenderWindowHandle() const;
    // Cached at Init() from SDL_GetRendererName (stable for the lifetime of this renderer).
    const std::string& GetName() const { return m_name; }

    bool GetOutputSize(int* w, int* h);
    bool GetCurrentOutputSize(int* w, int* h);

    bool SetRenderTarget(GuiTexture* texture);
    bool SetRenderToWindow() { return SetRenderTarget(nullptr); }
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderTarget
    SDL_Texture* GetRenderTarget() const;

    bool SetLogicalPresentation(int w, int h, SDL_RendererLogicalPresentation mode);
    bool GetLogicalPresentation(int* w, int* h, SDL_RendererLogicalPresentation* mode);
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderLogicalPresentationRect
    bool GetLogicalPresentationRect(SDL_FRect* rect);
    bool GetLogicalPresentationRect(SDL_FRect& rect) { return GetLogicalPresentationRect(&rect); }

    bool TransformWindowCoordToRender(float windowX, float windowY, float* x, float* y);
    bool TransformRenderCoordToWindow(float x, float y, float* windowX, float* windowY);
    bool TransformCoordToRender(SDL_Event& event);

    bool SetViewport(const SDL_Rect* rect);
    bool SetViewportFull();
    bool GetViewport(SDL_Rect* rect);
    bool SetViewport(const SDL_Rect& rect) { return SetViewport(&rect); }
    bool GetViewport(SDL_Rect& rect) { return GetViewport(&rect); }
    bool HasViewportSet();
    // https://wiki.libsdl.org/SDL3/SDL_GetRenderSafeArea
    bool GetSafeArea(SDL_Rect* rect);
    bool GetSafeArea(SDL_Rect& rect) { return GetSafeArea(&rect); }
    bool SetClipRect(const SDL_Rect* rect);
    bool DisableClipping();
    bool GetClipRect(SDL_Rect* rect);
    bool SetClipRect(const SDL_Rect& rect) { return SetClipRect(&rect); }
    bool GetClipRect(SDL_Rect& rect) { return GetClipRect(&rect); }
    bool IsClipEnabled();
    bool SetRenderScale(float scaleX, float scaleY);
    bool GetRenderScale(float* scaleX, float* scaleY);

    bool SetRenderDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    bool SetRenderDrawColor(float r, float g, float b, float a);
    bool GetRenderDrawColor(Uint8* r, Uint8* g, Uint8* b, Uint8* a);
    bool GetRenderDrawColor(float* r, float* g, float* b, float* a);

    bool SetColorScale(float scale);
    bool GetColorScale(float* scale);
    bool SetBlendMode(SDL_BlendMode blendMode);
    bool GetBlendMode(SDL_BlendMode* blendMode);

    bool Clear();

    bool DrawPoint(float x, float y);
    bool DrawPoints(const SDL_FPoint* points, int count);
    bool DrawLine(float x1, float y1, float x2, float y2);
    bool DrawLines(const SDL_FPoint* points, int count);
    bool DrawRect(const SDL_FRect* rect);
    bool DrawRects(const SDL_FRect* rects, int count);
    bool FillRect(const SDL_FRect* rect);
    bool FillRects(const SDL_FRect* rects, int count);

    bool DrawTexture(GuiTexture* texture, const SDL_FRect* srcRect, const SDL_FRect* dstRect);
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
    // Texture may be nullptr (vertex colors only). https://wiki.libsdl.org/SDL3/SDL_RenderGeometry
    bool RenderGeometry(GuiTexture* texture, const SDL_Vertex* vertices, int numVertices,
                        const int* indices, int numIndices);
    bool RenderGeometryRaw(GuiTexture* texture, const float* xy, int xyStride,
                           const SDL_FColor* color, int colorStride, const float* uv, int uvStride,
                           int numVertices, const void* indices, int numIndices, int indexType);
    // https://wiki.libsdl.org/SDL3/SDL_SetRenderTextureAddressMode
    bool SetTextureAddressMode(SDL_TextureAddressMode uMode, SDL_TextureAddressMode vMode);
    bool GetTextureAddressMode(SDL_TextureAddressMode* uMode, SDL_TextureAddressMode* vMode);
    // https://wiki.libsdl.org/SDL3/SDL_SetDefaultTextureScaleMode
    bool SetDefaultTextureScaleMode(SDL_ScaleMode scaleMode);
    bool GetDefaultTextureScaleMode(SDL_ScaleMode* scaleMode);

    // https://wiki.libsdl.org/SDL3/SDL_RenderReadPixels
    Ref<Surface> ReadPixels(const SDL_Rect* rect);
    Ref<Surface> ReadPixels(const SDL_Rect& rect) { return ReadPixels(&rect); }

    bool Present();
    bool Flush();

    bool SetVSync(int vsync);
    bool GetVSync(int* vsync);

    // https://wiki.libsdl.org/SDL3/SDL_RenderDebugText (8x8 px glyphs; SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE).
    bool DrawDebugText(float x, float y, const char* text);
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
    // https://wiki.libsdl.org/SDL3/SDL_GetGPURendererDevice (nullptr if this is not a GPU renderer).
    SDL_GPUDevice* GetGPUDevice() const;
    // https://wiki.libsdl.org/SDL3/SDL_CreateGPURenderState
    SDL_GPURenderState* CreateGPURenderState(const SDL_GPURenderStateCreateInfo* createInfo);
    bool SetGPURenderState(SDL_GPURenderState* state);
    void DestroyGPURenderState(SDL_GPURenderState* state);
    static bool SetGPURenderStateFragmentUniforms(SDL_GPURenderState* state, Uint32 slotIndex,
                                                  const void* data, Uint32 length);
#endif
#if SDL_VERSION_ATLEAST(3, 6, 0)
    static bool SetGPURenderStateSamplerBindings(SDL_GPURenderState* state, int numSamplerBindings,
                                                 const SDL_GPUTextureSamplerBinding* samplerBindings);
    static bool SetGPURenderStateStorageTextures(SDL_GPURenderState* state, int numStorageTextures,
                                                 SDL_GPUTexture* const* storageTextures);
    static bool SetGPURenderStateStorageBuffers(SDL_GPURenderState* state, int numStorageBuffers,
                                                SDL_GPUBuffer* const* storageBuffers);
#endif

#ifdef SDL_PLATFORM_GDK
    void GDKSuspendRenderer();
    void GDKResumeRenderer();
#endif

private:
    Ref<Window> m_window;
    SDL_Renderer* m_handle = nullptr;
    std::string m_name;

}; // class GuiRenderer

} // namespace rad
