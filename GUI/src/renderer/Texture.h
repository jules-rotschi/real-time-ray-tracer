#pragma once

#include <stdint.h> // uint32_t

#include <glad/gl.h> // GLuint

namespace GUI::Renderer
{
	class Texture
	{
	public:
		Texture(int width, int height);

		GLuint GetHandle() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		GLuint m_Handle = 0;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
	};
}