#pragma once

#include <stdint.h> // uint32_t

#include <glad/gl.h> // GLuint

namespace GUI::Renderer
{
	class Texture
	{
	public:
		enum class FilteringMode
		{
			NEAREST,
			LINEAR
		};

	public:
		Texture(FilteringMode filteringMode = FilteringMode::NEAREST);

		Texture(const Texture&) = delete;
		Texture(Texture&& other) noexcept;
		
		Texture(uint32_t width, uint32_t height, FilteringMode filteringMode = FilteringMode::NEAREST);

		~Texture();

		void Init(uint32_t width, uint32_t height);
		void ShutDown();

		void Resize(uint32_t width, uint32_t height);

		GLuint GetHandle() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

		Texture& operator=(const Texture&) = delete;
		Texture& operator=(Texture&& other) noexcept;
		
	private:
		void Init();

	private:
		GLuint m_Handle = 0;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		FilteringMode m_FilteringMode = FilteringMode::LINEAR;
	};
}