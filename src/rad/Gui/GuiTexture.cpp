#include <rad/Common/MemoryDebug.h>

#include <rad/Gui/GuiTexture.h>

#include <rad/Gui/GuiRenderer.h>

namespace rad
{

Ref<GuiTexture> GuiTexture::Create(Ref<GuiRenderer> renderer, SDL_PixelFormat format,
                                   SDL_TextureAccess access, int w, int h)
{
    if (SDL_Texture* handle = SDL_CreateTexture(renderer->GetHandle(), format, access, w, h))
    {
        return RAD_NEW GuiTexture(std::move(renderer), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateTexture failed: {}", SDL_GetError());
    return nullptr;
}

Ref<GuiTexture> GuiTexture::CreateWithProperties(Ref<GuiRenderer> renderer, SDL_PropertiesID props)
{
    if (SDL_Texture* handle = SDL_CreateTextureWithProperties(renderer->GetHandle(), props))
    {
        return RAD_NEW GuiTexture(std::move(renderer), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateTextureWithProperties failed: {}", SDL_GetError());
    return nullptr;
}

Ref<GuiTexture> GuiTexture::CreateFromSurface(Ref<GuiRenderer> renderer, SDL_Surface* surface)
{
    if (SDL_Texture* handle = SDL_CreateTextureFromSurface(renderer->GetHandle(), surface))
    {
        return RAD_NEW GuiTexture(std::move(renderer), handle);
    }
    RAD_LOG_GUI(err, "SDL_CreateTextureFromSurface failed: {}", SDL_GetError());
    return nullptr;
}

GuiTexture::GuiTexture(Ref<GuiRenderer> renderer, SDL_Texture* handle) :
    m_renderer(std::move(renderer)),
    m_handle(handle)
{
    assert(m_handle != nullptr);
    if (SDL_GetTextureProperties(m_handle) == 0)
    {
        RAD_LOG_GUI(err, "SDL_GetTextureProperties failed: {}", SDL_GetError());
    }
}

GuiTexture::~GuiTexture()
{
    Destroy();
}

void GuiTexture::Destroy()
{
    if (!m_handle)
    {
        return;
    }
    if (m_renderer->GetRenderTarget() == m_handle)
    {
        m_renderer->SetRenderTarget(nullptr);
    }
    SDL_DestroyTexture(m_handle);
    m_handle = nullptr;
}

SDL_PropertiesID GuiTexture::GetProperties() const
{
    return SDL_GetTextureProperties(m_handle);
}

SDL_Renderer* GuiTexture::GetRendererHandle() const
{
    return SDL_GetRendererFromTexture(m_handle);
}

bool GuiTexture::GetSize(float* w, float* h)
{
    return SDL_CHECK(SDL_GetTextureSize(m_handle, w, h));
}

bool GuiTexture::GetDimensions(int* w, int* h)
{
    const SDL_PropertiesID props = GetProperties();
    if (props == 0)
    {
        return false;
    }
    if (w != nullptr)
    {
        *w = (int)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
    }
    if (h != nullptr)
    {
        *h = (int)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);
    }
    return true;
}

SDL_PixelFormat GuiTexture::GetPixelFormat() const
{
    const SDL_PropertiesID props = GetProperties();
    if (props == 0)
    {
        return (SDL_PixelFormat)0;
    }
    return (SDL_PixelFormat)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_FORMAT_NUMBER,
                                                  (Sint64)SDL_PIXELFORMAT_UNKNOWN);
}

SDL_TextureAccess GuiTexture::GetAccess() const
{
    const SDL_PropertiesID props = GetProperties();
    if (props == 0)
    {
        return (SDL_TextureAccess)0;
    }
    return (SDL_TextureAccess)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_ACCESS_NUMBER, 0);
}

SDL_Colorspace GuiTexture::GetColorspace() const
{
    const SDL_PropertiesID props = GetProperties();
    if (props == 0)
    {
        return (SDL_Colorspace)0;
    }
    return (SDL_Colorspace)SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_COLORSPACE_NUMBER, 0);
}

bool GuiTexture::SetColorMod(Uint8 r, Uint8 g, Uint8 b)
{
    return SDL_CHECK(SDL_SetTextureColorMod(m_handle, r, g, b));
}

bool GuiTexture::SetColorMod(float r, float g, float b)
{
    return SDL_CHECK(SDL_SetTextureColorModFloat(m_handle, r, g, b));
}

bool GuiTexture::GetColorMod(Uint8* r, Uint8* g, Uint8* b)
{
    return SDL_CHECK(SDL_GetTextureColorMod(m_handle, r, g, b));
}

bool GuiTexture::GetColorMod(float* r, float* g, float* b)
{
    return SDL_CHECK(SDL_GetTextureColorModFloat(m_handle, r, g, b));
}

bool GuiTexture::SetAlphaMod(Uint8 alpha)
{
    return SDL_CHECK(SDL_SetTextureAlphaMod(m_handle, alpha));
}

bool GuiTexture::SetAlphaMod(float alpha)
{
    return SDL_CHECK(SDL_SetTextureAlphaModFloat(m_handle, alpha));
}

bool GuiTexture::GetAlphaMod(Uint8* alpha)
{
    return SDL_CHECK(SDL_GetTextureAlphaMod(m_handle, alpha));
}

bool GuiTexture::GetAlphaMod(float* alpha)
{
    return SDL_CHECK(SDL_GetTextureAlphaModFloat(m_handle, alpha));
}

bool GuiTexture::SetBlendMode(SDL_BlendMode blendMode)
{
    return SDL_CHECK(SDL_SetTextureBlendMode(m_handle, blendMode));
}

bool GuiTexture::GetBlendMode(SDL_BlendMode* blendMode)
{
    return SDL_CHECK(SDL_GetTextureBlendMode(m_handle, blendMode));
}

bool GuiTexture::SetScaleMode(SDL_ScaleMode scaleMode)
{
    return SDL_CHECK(SDL_SetTextureScaleMode(m_handle, scaleMode));
}

bool GuiTexture::GetScaleMode(SDL_ScaleMode* scaleMode)
{
    return SDL_CHECK(SDL_GetTextureScaleMode(m_handle, scaleMode));
}

bool GuiTexture::SetSDRWhitePoint(float whitePoint)
{
    const SDL_PropertiesID props = GetProperties();
    if (props == 0)
    {
        return false;
    }
    return SDL_CHECK(
        SDL_SetFloatProperty(props, SDL_PROP_TEXTURE_SDR_WHITE_POINT_FLOAT, whitePoint));
}

bool GuiTexture::SetHDRHeadroom(float headroom)
{
    const SDL_PropertiesID props = GetProperties();
    if (props == 0)
    {
        return false;
    }
    return SDL_CHECK(SDL_SetFloatProperty(props, SDL_PROP_TEXTURE_HDR_HEADROOM_FLOAT, headroom));
}

float GuiTexture::GetSDRWhitePoint(float defaultValue) const
{
    const SDL_PropertiesID props = GetProperties();
    if (props == 0)
    {
        return defaultValue;
    }
    return SDL_GetFloatProperty(props, SDL_PROP_TEXTURE_SDR_WHITE_POINT_FLOAT, defaultValue);
}

float GuiTexture::GetHDRHeadroom(float defaultValue) const
{
    const SDL_PropertiesID props = GetProperties();
    if (props == 0)
    {
        return defaultValue;
    }
    return SDL_GetFloatProperty(props, SDL_PROP_TEXTURE_HDR_HEADROOM_FLOAT, defaultValue);
}

bool GuiTexture::SetPalette(SDL_Palette* palette)
{
    return SDL_CHECK(SDL_SetTexturePalette(m_handle, palette));
}

SDL_Palette* GuiTexture::GetPalette() const
{
    return SDL_GetTexturePalette(m_handle);
}

bool GuiTexture::Update(const SDL_Rect* rect, const void* pixels, int pitch)
{
    return SDL_CHECK(SDL_UpdateTexture(m_handle, rect, pixels, pitch));
}

bool GuiTexture::UpdateYUV(const SDL_Rect* rect, const Uint8* Yplane, int Ypitch,
                           const Uint8* Uplane, int Upitch, const Uint8* Vplane, int Vpitch)
{
    return SDL_CHECK(
        SDL_UpdateYUVTexture(m_handle, rect, Yplane, Ypitch, Uplane, Upitch, Vplane, Vpitch));
}

bool GuiTexture::UpdateNV(const SDL_Rect* rect, const Uint8* Yplane, int Ypitch,
                          const Uint8* UVplane, int UVpitch)
{
    return SDL_CHECK(SDL_UpdateNVTexture(m_handle, rect, Yplane, Ypitch, UVplane, UVpitch));
}

bool GuiTexture::Lock(const SDL_Rect* rect, void** pixels, int* pitch)
{
    return SDL_CHECK(SDL_LockTexture(m_handle, rect, pixels, pitch));
}

bool GuiTexture::LockToSurface(const SDL_Rect* rect, SDL_Surface** surface)
{
    return SDL_CHECK(SDL_LockTextureToSurface(m_handle, rect, surface));
}

void GuiTexture::Unlock()
{
    if (m_handle)
    {
        SDL_UnlockTexture(m_handle);
    }
}

} // namespace rad
