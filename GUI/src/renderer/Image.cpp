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
        GLuint texture = m_Texture.GetHandle();
        glDeleteTextures(1, &texture);
        m_Texture = Texture(width, height);
    }

    void Image::SetData(const uint8_t* pixelBuffer) const
    {
        glBindTexture(GL_TEXTURE_2D, m_Texture.GetHandle());
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0, 0,
            GetWidth(),
            GetHeight(),
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixelBuffer
        );
    }

    ImTextureID Image::GetTexture() const
    {
        return m_Texture.GetHandle();
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