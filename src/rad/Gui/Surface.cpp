#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/Surface.h>

namespace rad
{

Ref<Surface> Surface::Create(int width, int height, SDL_PixelFormat format)
{
    if (SDL_Surface* handle = SDL_CreateSurface(width, height, format))
    {
        return RAD_NEW Surface(handle, true);
    }
    RAD_LOG_GUI(err, "SDL_CreateSurface failed: {}", SDL_GetError());
    return nullptr;
}

Ref<Surface> Surface::CreateFromPixels(int width, int height, SDL_PixelFormat format, void* pixels,
                                       int pitch)
{
    if (SDL_Surface* handle = SDL_CreateSurfaceFrom(width, height, format, pixels, pitch))
    {
        return RAD_NEW Surface(handle, true);
    }
    RAD_LOG_GUI(err, "SDL_CreateSurfaceFrom failed: {}", SDL_GetError());
    return nullptr;
}

Ref<Surface> Surface::CreateFromBMP(SDL_IOStream* src, bool closeio)
{
    if (SDL_Surface* handle = SDL_LoadBMP_IO(src, closeio))
    {
        return RAD_NEW Surface(handle, true);
    }
    RAD_LOG_GUI(err, "SDL_LoadBMP_IO failed: {}", SDL_GetError());
    return nullptr;
}

Ref<Surface> Surface::CreateFromBMP(cstring_view file)
{
    if (SDL_Surface* handle = SDL_LoadBMP(file.c_str()))
    {
        return RAD_NEW Surface(handle, true);
    }
    RAD_LOG_GUI(err, "SDL_LoadBMP failed: {}", SDL_GetError());
    return nullptr;
}

Surface::Surface(SDL_Surface* handle, bool isManaged) : m_handle(handle), m_isManaged(isManaged)
{
    if (!m_handle)
    {
        RAD_LOG_GUI(err, "Surface constructed with null SDL_Surface.");
        return;
    }
    m_propID = SDL_GetSurfaceProperties(m_handle);
    if (m_propID == 0)
    {
        RAD_LOG_GUI(err, "SDL_GetSurfaceProperties failed: {}", SDL_GetError());
    }
}

Surface::~Surface()
{
    Destroy();
}

void Surface::Destroy()
{
    if (m_handle && m_isManaged)
    {
        SDL_DestroySurface(m_handle);
        m_handle = nullptr;
        m_propID = 0;
    }
}

bool Surface::SaveBMP(SDL_IOStream* dst, bool closeio)
{
    return SDL_CHECK(SDL_SaveBMP_IO(m_handle, dst, closeio));
}

bool Surface::SaveBMP(cstring_view file)
{
    return SDL_CHECK(SDL_SaveBMP(m_handle, file.c_str()));
}

bool Surface::SetColorSpace(SDL_Colorspace colorspace)
{
    return SDL_CHECK(SDL_SetSurfaceColorspace(m_handle, colorspace));
}

SDL_Colorspace Surface::GetColorSpace() const
{
    return SDL_GetSurfaceColorspace(m_handle);
}

bool Surface::SetPalette(SDL_Palette* palette)
{
    return SDL_CHECK(SDL_SetSurfacePalette(m_handle, palette));
}

bool Surface::Lock()
{
    return SDL_CHECK(SDL_LockSurface(m_handle));
}

void Surface::Unlock()
{
    SDL_UnlockSurface(m_handle);
}

bool Surface::SetRLE(int flag)
{
    return SDL_CHECK(SDL_SetSurfaceRLE(m_handle, flag));
}

bool Surface::HasRLE()
{
    return SDL_SurfaceHasRLE(m_handle);
}

bool Surface::SetColorKey(int flag, Uint32 key)
{
    return SDL_CHECK(SDL_SetSurfaceColorKey(m_handle, flag, key));
}

bool Surface::HasColorKey()
{
    return SDL_SurfaceHasColorKey(m_handle);
}

bool Surface::GetColorKey(Uint32* key)
{
    return SDL_CHECK(SDL_GetSurfaceColorKey(m_handle, key));
}

bool Surface::SetColorMod(Uint8 r, Uint8 g, Uint8 b)
{
    return SDL_CHECK(SDL_SetSurfaceColorMod(m_handle, r, g, b));
}

bool Surface::GetColorMod(Uint8* r, Uint8* g, Uint8* b)
{
    return SDL_CHECK(SDL_GetSurfaceColorMod(m_handle, r, g, b));
}

bool Surface::SetAlphaMod(Uint8 alpha)
{
    return SDL_CHECK(SDL_SetSurfaceAlphaMod(m_handle, alpha));
}

bool Surface::GetAlphaMod(Uint8* alpha)
{
    return SDL_CHECK(SDL_GetSurfaceAlphaMod(m_handle, alpha));
}

bool Surface::SetBlendMode(SDL_BlendMode blendMode)
{
    return SDL_CHECK(SDL_SetSurfaceBlendMode(m_handle, blendMode));
}

bool Surface::GetBlendMode(SDL_BlendMode* blendMode)
{
    return SDL_CHECK(SDL_GetSurfaceBlendMode(m_handle, blendMode));
}

bool Surface::SetClipRect(const SDL_Rect* rect)
{
    return SDL_CHECK(SDL_SetSurfaceClipRect(m_handle, rect));
}

bool Surface::DisableClipping()
{
    return SetClipRect(nullptr);
}

bool Surface::GetClipRect(SDL_Rect* rect)
{
    return SDL_CHECK(SDL_GetSurfaceClipRect(m_handle, rect));
}

bool Surface::Flip(SDL_FlipMode flip)
{
    return SDL_CHECK(SDL_FlipSurface(m_handle, flip));
}

Ref<Surface> Surface::Duplicate()
{
    if (SDL_Surface* handle = SDL_DuplicateSurface(m_handle))
    {
        return Ref<Surface>(RAD_NEW Surface(handle, true));
    }
    RAD_LOG_GUI(err, "SDL_DuplicateSurface failed: {}", SDL_GetError());
    return nullptr;
}

Ref<Surface> Surface::Convert(SDL_PixelFormat format)
{
    if (SDL_Surface* handle = SDL_ConvertSurface(m_handle, format))
    {
        return Ref<Surface>(RAD_NEW Surface(handle, true));
    }
    RAD_LOG_GUI(err, "SDL_ConvertSurface failed: {}", SDL_GetError());
    return nullptr;
}

Ref<Surface> Surface::Convert(SDL_PixelFormat format, SDL_Palette* palette,
                              SDL_Colorspace colorspace, SDL_PropertiesID props)
{
    if (SDL_Surface* handle =
            SDL_ConvertSurfaceAndColorspace(m_handle, format, palette, colorspace, props))
    {
        return Ref<Surface>(RAD_NEW Surface(handle, true));
    }
    RAD_LOG_GUI(err, "SDL_ConvertSurfaceAndColorspace failed: {}", SDL_GetError());
    return nullptr;
}

bool Surface::PremultiplyAlpha(bool linear)
{
    return SDL_CHECK(SDL_PremultiplySurfaceAlpha(m_handle, linear));
}

bool Surface::Clear(float r, float g, float b, float a)
{
    return SDL_CHECK(SDL_ClearSurface(m_handle, r, g, b, a));
}

bool Surface::FillRect(const SDL_Rect* rect, Uint32 color)
{
    return SDL_CHECK(SDL_FillSurfaceRect(m_handle, rect, color));
}

bool Surface::FillRects(const SDL_Rect* rects, int count, Uint32 color)
{
    return SDL_CHECK(SDL_FillSurfaceRects(m_handle, rects, count, color));
}

bool Surface::Blit(Surface* src, const SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect)
{
    return SDL_CHECK(SDL_BlitSurface(src->GetHandle(), srcRect, dst->GetHandle(), dstRect));
}

bool Surface::BlitUnchecked(Surface* src, const SDL_Rect* srcRect, Surface* dst,
                            const SDL_Rect* dstRect)
{
    return SDL_CHECK(SDL_BlitSurfaceUnchecked(src->GetHandle(), srcRect, dst->GetHandle(), dstRect));
}

bool Surface::BlitScaled(Surface* src, const SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect,
                         SDL_ScaleMode scaleMode)
{
    return SDL_CHECK(
        SDL_BlitSurfaceScaled(src->GetHandle(), srcRect, dst->GetHandle(), dstRect, scaleMode));
}

bool Surface::BlitUncheckedScaled(Surface* src, const SDL_Rect* srcRect, Surface* dst,
                                SDL_Rect* dstRect, SDL_ScaleMode scaleMode)
{
    return SDL_CHECK(SDL_BlitSurfaceUncheckedScaled(src->GetHandle(), srcRect, dst->GetHandle(),
                                                  dstRect, scaleMode));
}

bool Surface::BlitTiled(Surface* src, const SDL_Rect* srcRect, Surface* dst, SDL_Rect* dstRect)
{
    return SDL_CHECK(SDL_BlitSurfaceTiled(src->GetHandle(), srcRect, dst->GetHandle(), dstRect));
}

bool Surface::BlitTiledWithScale(Surface* src, const SDL_Rect* srcRect, float scale,
                                 SDL_ScaleMode scaleMode, Surface* dst, const SDL_Rect* dstRect)
{
    return SDL_CHECK(SDL_BlitSurfaceTiledWithScale(src->GetHandle(), srcRect, scale, scaleMode,
                                                   dst->GetHandle(), dstRect));
}

bool Surface::Blit9Grid(Surface* src, const SDL_Rect* srcRect, int leftWidth, int rightWidth,
                        int topHeight, int bottomHeight, float scale, SDL_ScaleMode scaleMode,
                        Surface* dst, const SDL_Rect* dstRect)
{
    return SDL_CHECK(SDL_BlitSurface9Grid(src->GetHandle(), srcRect, leftWidth, rightWidth,
                                          topHeight, bottomHeight, scale, scaleMode,
                                          dst->GetHandle(), dstRect));
}

bool Surface::ReadPixel(int x, int y, Uint8* r, Uint8* g, Uint8* b, Uint8* a)
{
    return SDL_CHECK(SDL_ReadSurfacePixel(m_handle, x, y, r, g, b, a));
}

} // namespace rad
