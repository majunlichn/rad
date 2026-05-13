#pragma once

#include <rad/Gui/GuiCommon.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

namespace rad
{

class GuiRenderer;

// GPU-backed texture for GuiRenderer. SDL render APIs are main-thread-only; see SDL issue #986.
// https://wiki.libsdl.org/SDL3/CategoryRender
// https://github.com/libsdl-org/SDL/issues/986
class GuiTexture : public RefCounted<GuiTexture>
{
public:
    // https://wiki.libsdl.org/SDL3/SDL_CreateTexture
    static Ref<GuiTexture> Create(Ref<GuiRenderer> renderer, SDL_PixelFormat format,
                                  SDL_TextureAccess access, int w, int h);
    // https://wiki.libsdl.org/SDL3/SDL_CreateTextureWithProperties
    static Ref<GuiTexture> CreateWithProperties(Ref<GuiRenderer> renderer, SDL_PropertiesID props);
    // https://wiki.libsdl.org/SDL3/SDL_CreateTextureFromSurface
    static Ref<GuiTexture> CreateFromSurface(Ref<GuiRenderer> renderer, SDL_Surface* surface);

    // Queries SDL_GetTextureProperties for the new handle (logs if unavailable).
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    GuiTexture(Ref<GuiRenderer> renderer, SDL_Texture* handle);
    // Calls Destroy().
    // https://wiki.libsdl.org/SDL3/SDL_DestroyTexture
    ~GuiTexture();
    // If this texture is the current render target, clears it; then SDL_DestroyTexture.
    // https://wiki.libsdl.org/SDL3/SDL_DestroyTexture
    void Destroy();

    // Returns m_handle.
    SDL_Texture* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    SDL_PropertiesID GetProperties() const;

    // https://wiki.libsdl.org/SDL3/SDL_GetRendererFromTexture
    SDL_Renderer* GetRendererHandle() const;

    // https://wiki.libsdl.org/SDL3/SDL_GetTextureSize
    bool GetSize(float* w, float* h);
    // Integer size from SDL_PROP_TEXTURE_WIDTH_NUMBER / HEIGHT_NUMBER via SDL_GetNumberProperty.
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    // https://wiki.libsdl.org/SDL3/SDL_GetNumberProperty
    bool GetDimensions(int* w, int* h);

    // SDL_PROP_TEXTURE_FORMAT_NUMBER via texture properties.
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    SDL_PixelFormat GetPixelFormat() const;
    // SDL_PROP_TEXTURE_ACCESS_NUMBER via texture properties.
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    SDL_TextureAccess GetAccess() const;
    // SDL_PROP_TEXTURE_COLORSPACE_NUMBER via texture properties.
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    SDL_Colorspace GetColorspace() const;

    // https://wiki.libsdl.org/SDL3/SDL_SetTextureColorMod
    bool SetColorMod(Uint8 r, Uint8 g, Uint8 b);
    // https://wiki.libsdl.org/SDL3/SDL_SetTextureColorModFloat
    bool SetColorMod(float r, float g, float b);
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureColorMod
    bool GetColorMod(Uint8* r, Uint8* g, Uint8* b);
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureColorModFloat
    bool GetColorMod(float* r, float* g, float* b);
    // https://wiki.libsdl.org/SDL3/SDL_SetTextureAlphaMod
    bool SetAlphaMod(Uint8 alpha);
    // https://wiki.libsdl.org/SDL3/SDL_SetTextureAlphaModFloat
    bool SetAlphaMod(float alpha);
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureAlphaMod
    bool GetAlphaMod(Uint8* alpha);
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureAlphaModFloat
    bool GetAlphaMod(float* alpha);
    // https://wiki.libsdl.org/SDL3/SDL_SetTextureBlendMode
    bool SetBlendMode(SDL_BlendMode blendMode);
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureBlendMode
    bool GetBlendMode(SDL_BlendMode* blendMode);
    // https://wiki.libsdl.org/SDL3/SDL_SetTextureScaleMode
    bool SetScaleMode(SDL_ScaleMode scaleMode);
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureScaleMode
    bool GetScaleMode(SDL_ScaleMode* scaleMode);

    // SDL_PROP_TEXTURE_SDR_WHITE_POINT_FLOAT on the texture property set.
    // https://wiki.libsdl.org/SDL3/SDL_SetFloatProperty
    bool SetSDRWhitePoint(float whitePoint);
    // SDL_PROP_TEXTURE_HDR_HEADROOM_FLOAT on the texture property set.
    // https://wiki.libsdl.org/SDL3/SDL_SetFloatProperty
    bool SetHDRHeadroom(float headroom);
    // https://wiki.libsdl.org/SDL3/SDL_GetFloatProperty
    float GetSDRWhitePoint(float defaultValue) const;
    // https://wiki.libsdl.org/SDL3/SDL_GetFloatProperty
    float GetHDRHeadroom(float defaultValue) const;

    // https://wiki.libsdl.org/SDL3/SDL_SetTexturePalette
    bool SetPalette(SDL_Palette* palette);
    // https://wiki.libsdl.org/SDL3/SDL_GetTexturePalette
    SDL_Palette* GetPalette() const;

    // https://wiki.libsdl.org/SDL3/SDL_UpdateTexture
    bool Update(const SDL_Rect* rect, const void* pixels, int pitch);
    // https://wiki.libsdl.org/SDL3/SDL_UpdateYUVTexture
    bool UpdateYUV(const SDL_Rect* rect, const Uint8* Yplane, int Ypitch, const Uint8* Uplane,
                   int Upitch, const Uint8* Vplane, int Vpitch);
    // https://wiki.libsdl.org/SDL3/SDL_UpdateNVTexture
    bool UpdateNV(const SDL_Rect* rect, const Uint8* Yplane, int Ypitch, const Uint8* UVplane,
                  int UVpitch);

    // https://wiki.libsdl.org/SDL3/SDL_LockTexture
    bool Lock(const SDL_Rect* rect, void** pixels, int* pitch);
    // https://wiki.libsdl.org/SDL3/SDL_LockTextureToSurface
    bool LockToSurface(const SDL_Rect* rect, SDL_Surface** surface);
    // https://wiki.libsdl.org/SDL3/SDL_UnlockTexture
    void Unlock();

private:
    Ref<GuiRenderer> m_renderer;
    SDL_Texture* m_handle = nullptr;

}; // class GuiTexture

} // namespace rad
