#include "Texture.h"

#include <string>

namespace GUI::Renderer
{
	Texture::Texture(int width, int height)
		: m_Width(width), m_Height(height)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

		glTextureStorage2D(m_Handle, 1, GL_RGBA32F, width, height);

		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	GLuint Texture::GetHandle() const
	{
		return m_Handle;
	}

	uint32_t Texture::GetWidth() const
	{
		return m_Width;
	}

	uint32_t Texture::GetHeight() const
	{
		return m_Height;
	}
}