#pragma once

#include <rad/Gui/GuiCommon.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>

namespace rad
{

class GuiRenderer;

// GPU-backed texture for GuiRenderer.
// Should only be used on the main thread: https://github.com/libsdl-org/SDL/issues/986
class GuiTexture : public RefCounted<GuiTexture>
{
public:
    static Ref<GuiTexture> Create(Ref<GuiRenderer> renderer, SDL_PixelFormat format,
                                  SDL_TextureAccess access, int w, int h);
    static Ref<GuiTexture> CreateWithProperties(Ref<GuiRenderer> renderer, SDL_PropertiesID props);
    static Ref<GuiTexture> CreateFromSurface(Ref<GuiRenderer> renderer, SDL_Surface* surface);

    GuiTexture(Ref<GuiRenderer> renderer, SDL_Texture* handle);
    ~GuiTexture();
    void Destroy();

    SDL_Texture* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    SDL_PropertiesID GetProperties() const;

    SDL_Renderer* GetRendererHandle() const;

    bool GetSize(float* w, float* h);
    // Integer width and height from SDL_PROP_TEXTURE_WIDTH_NUMBER / HEIGHT_NUMBER.
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    bool GetDimensions(int* w, int* h);

    SDL_PixelFormat GetPixelFormat() const;
    SDL_TextureAccess GetAccess() const;
    SDL_Colorspace GetColorspace() const;

    bool SetColorMod(Uint8 r, Uint8 g, Uint8 b);
    bool SetColorMod(float r, float g, float b);
    bool GetColorMod(Uint8* r, Uint8* g, Uint8* b);
    bool GetColorMod(float* r, float* g, float* b);
    bool SetAlphaMod(Uint8 alpha);
    bool SetAlphaMod(float alpha);
    bool GetAlphaMod(Uint8* alpha);
    bool GetAlphaMod(float* alpha);
    bool SetBlendMode(SDL_BlendMode blendMode);
    bool GetBlendMode(SDL_BlendMode* blendMode);
    bool SetScaleMode(SDL_ScaleMode scaleMode);
    bool GetScaleMode(SDL_ScaleMode* scaleMode);

    // HDR10 and floating-point textures: SDL_PROP_TEXTURE_SDR_WHITE_POINT_FLOAT and
    // SDL_PROP_TEXTURE_HDR_HEADROOM_FLOAT on the texture property set.
    // https://wiki.libsdl.org/SDL3/SDL_GetTextureProperties
    bool SetSDRWhitePoint(float whitePoint);
    bool SetHDRHeadroom(float headroom);
    float GetSDRWhitePoint(float defaultValue) const;
    float GetHDRHeadroom(float defaultValue) const;

    bool SetPalette(SDL_Palette* palette);
    SDL_Palette* GetPalette() const;

    bool Update(const SDL_Rect* rect, const void* pixels, int pitch);
    bool UpdateYUV(const SDL_Rect* rect, const Uint8* Yplane, int Ypitch, const Uint8* Uplane,
                   int Upitch, const Uint8* Vplane, int Vpitch);
    bool UpdateNV(const SDL_Rect* rect, const Uint8* Yplane, int Ypitch, const Uint8* UVplane,
                  int UVpitch);

    bool Lock(const SDL_Rect* rect, void** pixels, int* pitch);
    bool LockToSurface(const SDL_Rect* rect, SDL_Surface** surface);
    void Unlock();

private:
    Ref<GuiRenderer> m_renderer;
    SDL_Texture* m_handle = nullptr;

}; // class GuiTexture

} // namespace rad
