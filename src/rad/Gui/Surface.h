#pragma once

#include <rad/Common/String.h>
#include <rad/Gui/GuiCommon.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_surface.h>

namespace rad
{

// Reference-counted wrapper around SDL_Surface.
// https://wiki.libsdl.org/SDL3/CategorySurface
class Surface : public RefCounted<Surface>
{
public:
    // https://wiki.libsdl.org/SDL3/SDL_CreateSurface
    static Ref<Surface> Create(int width, int height, SDL_PixelFormat format);
    // https://wiki.libsdl.org/SDL3/SDL_CreateSurfaceFrom
    static Ref<Surface> CreateFromPixels(int width, int height, SDL_PixelFormat format,
                                         void* pixels, int pitch);
    // https://wiki.libsdl.org/SDL3/SDL_LoadBMP_IO
    static Ref<Surface> CreateFromBMP(SDL_IOStream* src, bool closeio);
    // https://wiki.libsdl.org/SDL3/SDL_LoadBMP
    static Ref<Surface> CreateFromBMP(cstring_view file);

    // Stores the handle and queries SDL_GetSurfaceProperties into m_propertiesId when non-null.
    // https://wiki.libsdl.org/SDL3/SDL_GetSurfaceProperties
    Surface(SDL_Surface* handle, bool isManaged);
    // Calls Destroy().
    // https://wiki.libsdl.org/SDL3/SDL_DestroySurface
    ~Surface();
    // https://wiki.libsdl.org/SDL3/SDL_DestroySurface
    void Destroy();

    // Returns m_handle.
    SDL_Surface* GetHandle() const { return m_handle; }

    // https://wiki.libsdl.org/SDL3/SDL_SaveBMP_IO
    bool SaveBMP(SDL_IOStream* dst, bool closeio);
    // https://wiki.libsdl.org/SDL3/SDL_SaveBMP
    bool SaveBMP(cstring_view file);

    // https://wiki.libsdl.org/SDL3/SDL_SetSurfaceColorspace
    bool SetColorSpace(SDL_Colorspace colorspace);
    // https://wiki.libsdl.org/SDL3/SDL_GetSurfaceColorspace
    SDL_Colorspace GetColorSpace() const;
    // https://wiki.libsdl.org/SDL3/SDL_SetSurfacePalette
    bool SetPalette(SDL_Palette* palette);

    // https://wiki.libsdl.org/SDL3/SDL_LockSurface
    bool Lock();
    // https://wiki.libsdl.org/SDL3/SDL_UnlockSurface
    void Unlock();

    // https://wiki.libsdl.org/SDL3/SDL_SetSurfaceRLE
    bool SetRLE(int flag);
    // https://wiki.libsdl.org/SDL3/SDL_SurfaceHasRLE
    bool HasRLE();
    // https://wiki.libsdl.org/SDL3/SDL_SetSurfaceColorKey
    bool SetColorKey(int flag, Uint32 key);
    // https://wiki.libsdl.org/SDL3/SDL_SurfaceHasColorKey
    bool HasColorKey();
    // https://wiki.libsdl.org/SDL3/SDL_GetSurfaceColorKey
    bool GetColorKey(Uint32* key);
    // https://wiki.libsdl.org/SDL3/SDL_SetSurfaceColorMod
    bool SetColorMod(Uint8 r, Uint8 g, Uint8 b);
    // https://wiki.libsdl.org/SDL3/SDL_GetSurfaceColorMod
    bool GetColorMod(Uint8* r, Uint8* g, Uint8* b);
    // https://wiki.libsdl.org/SDL3/SDL_SetSurfaceAlphaMod
    bool SetAlphaMod(Uint8 alpha);
    // https://wiki.libsdl.org/SDL3/SDL_GetSurfaceAlphaMod
    bool GetAlphaMod(Uint8* alpha);
    // https://wiki.libsdl.org/SDL3/SDL_SetSurfaceBlendMode
    bool SetBlendMode(SDL_BlendMode blendMode);
    // https://wiki.libsdl.org/SDL3/SDL_GetSurfaceBlendMode
    bool GetBlendMode(SDL_BlendMode* blendMode);
    // https://wiki.libsdl.org/SDL3/SDL_SetSurfaceClipRect
    bool SetClipRect(const SDL_Rect* rect);
    // Same as SetClipRect(nullptr).
    // https://wiki.libsdl.org/SDL3/SDL_SetSurfaceClipRect
    bool DisableClipping();
    // https://wiki.libsdl.org/SDL3/SDL_GetSurfaceClipRect
    bool GetClipRect(SDL_Rect* rect);
    // Delegates to SetClipRect(const SDL_Rect*).
    bool SetClipRect(const SDL_Rect& rect) { return SetClipRect(&rect); }
    // Delegates to GetClipRect(SDL_Rect*).
    bool GetClipRect(SDL_Rect& rect) { return GetClipRect(&rect); }

    // https://wiki.libsdl.org/SDL3/SDL_FlipSurface
    bool Flip(SDL_FlipMode flip);
    // https://wiki.libsdl.org/SDL3/SDL_DuplicateSurface
    Ref<Surface> Duplicate();
    // https://wiki.libsdl.org/SDL3/SDL_ConvertSurface
    Ref<Surface> Convert(SDL_PixelFormat format);
    // https://wiki.libsdl.org/SDL3/SDL_ConvertSurfaceAndColorspace
    Ref<Surface> Convert(SDL_PixelFormat format, SDL_Palette* palette, SDL_Colorspace colorspace,
                         SDL_PropertiesID props);

    // https://wiki.libsdl.org/SDL3/SDL_PremultiplySurfaceAlpha
    bool PremultiplyAlpha(bool linear);
    // https://wiki.libsdl.org/SDL3/SDL_ClearSurface
    bool Clear(float r, float g, float b, float a);

    // https://wiki.libsdl.org/SDL3/SDL_FillSurfaceRect
    bool FillRect(const SDL_Rect* rect, Uint32 color);
    // https://wiki.libsdl.org/SDL3/SDL_FillSurfaceRects
    bool FillRects(const SDL_Rect* rects, int count, Uint32 color);

    // https://wiki.libsdl.org/SDL3/SDL_BlitSurface
    static bool Blit(Surface* src, const SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect);
    // https://wiki.libsdl.org/SDL3/SDL_BlitSurfaceUnchecked
    static bool BlitUnchecked(Surface* src, const SDL_Rect* srcRect, Surface* dst,
                              const SDL_Rect* dstRect);
    // https://wiki.libsdl.org/SDL3/SDL_BlitSurfaceScaled
    static bool BlitScaled(Surface* src, const SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect,
                           SDL_ScaleMode scaleMode);
    // https://wiki.libsdl.org/SDL3/SDL_BlitSurfaceUncheckedScaled
    static bool BlitUncheckedScaled(Surface* src, const SDL_Rect* srcRect, Surface* dst,
                                     SDL_Rect* dstRect, SDL_ScaleMode scaleMode);
    // https://wiki.libsdl.org/SDL3/SDL_BlitSurfaceTiled
    static bool BlitTiled(Surface* src, const SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect);
    // https://wiki.libsdl.org/SDL3/SDL_BlitSurfaceTiledWithScale
    static bool BlitTiledWithScale(Surface* src, const SDL_Rect* srcRect, float scale,
                                   SDL_ScaleMode scaleMode, Surface* dst, const SDL_Rect* dstRect);
    // https://wiki.libsdl.org/SDL3/SDL_BlitSurface9Grid
    static bool Blit9Grid(Surface* src, const SDL_Rect* srcRect, int leftWidth, int rightWidth,
                          int topHeight, int bottomHeight, float scale, SDL_ScaleMode scaleMode,
                          Surface* dst, const SDL_Rect* dstRect);

    // https://wiki.libsdl.org/SDL3/SDL_ReadSurfacePixel
    bool ReadPixel(int x, int y, Uint8* r, Uint8* g, Uint8* b, Uint8* a);

private:
    SDL_Surface* m_handle = nullptr;
    bool m_isManaged = false;
    SDL_PropertiesID m_propertiesId = 0;

}; // class Surface

} // namespace rad
