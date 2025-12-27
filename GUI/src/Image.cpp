#include "Image.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace GUI
{
    Image::Image()
    {
        InitTexture();
    }

    void Image::InitTexture()
    {
        glGenTextures(1, &m_Texture);
        glBindTexture(GL_TEXTURE_2D, m_Texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void Image::Resize(uint32_t width, uint32_t height)
    {
        m_Width = width;
        m_Height = height;

        glDeleteTextures(1, &m_Texture);
        InitTexture();
    }

    void Image::SetData(const uint8_t* pixelBuffer) const
    {
        glBindTexture(GL_TEXTURE_2D, m_Texture);
        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0, 0,
            m_Width,
            m_Height,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            pixelBuffer
        );
    }

    ImTextureID Image::GetTexture() const
    {
        return (ImTextureID)(intptr_t)m_Texture;
    }

    uint32_t Image::GetWidth() const
    {
        return m_Width;
    }

    uint32_t Image::GetHeight() const
    {
        return m_Height;
    }
}