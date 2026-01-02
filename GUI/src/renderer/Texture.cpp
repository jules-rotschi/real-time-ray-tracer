#include "Texture.h"

#include <string>

namespace GUI::Renderer
{
	Texture::Texture(FilteringMode filteringMode)
		: m_FilteringMode(filteringMode)
	{
		Init();
	}

	Texture::Texture(uint32_t width, uint32_t height, FilteringMode filteringMode)
		: m_Width(width), m_Height(height), m_FilteringMode(filteringMode)
	{
		Init();
	}

	Texture::Texture(Texture&& other) noexcept
	{
		m_Handle = other.m_Handle;
		m_Width = other.m_Width;
		m_Height = other.m_Height;

		other.m_Handle = 0;
	}

	Texture::~Texture()
	{
		ShutDown();
	}

	void Texture::Init(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;
		Init();
	}

	void Texture::ShutDown()
	{
		glDeleteTextures(1, &m_Handle);
		m_Handle = 0;
	}

	void Texture::Resize(uint32_t width, uint32_t height)
	{
		ShutDown();
		Init(width, height);
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

	Texture& Texture::operator=(Texture&& other) noexcept
	{
		if (this != &other)
		{
			ShutDown();

			m_Handle = other.m_Handle;
			m_Width = other.m_Width;
			m_Height = other.m_Height;

			other.m_Handle = 0;
		}
		return *this;
	}

	void Texture::Init()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_Handle);

		glTextureStorage2D(m_Handle, 1, GL_RGBA32F, m_Width, m_Height);

		GLint filteringParam = GL_NEAREST;

		if (m_FilteringMode == FilteringMode::LINEAR)
			filteringParam = GL_LINEAR;

		glTextureParameteri(m_Handle, GL_TEXTURE_MIN_FILTER, filteringParam);
		glTextureParameteri(m_Handle, GL_TEXTURE_MAG_FILTER, filteringParam);

		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
}