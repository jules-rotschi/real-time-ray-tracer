#include "Image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace GUI::Renderer
{
    Image::Image(uint32_t width, uint32_t height)
        : m_Texture(width, height) {}

    void Image::Resize(uint32_t width, uint32_t height)
    {
        m_Texture.Resize(width, height);
    }

    const Texture& Image::GetTexture() const
    {
        return m_Texture;
    }

    uint32_t Image::GetWidth() const
    {
        return m_Texture.GetWidth();
    }

    uint32_t Image::GetHeight() const
    {
        return m_Texture.GetHeight();
    }
}