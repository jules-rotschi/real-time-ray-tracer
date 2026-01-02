#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Texture.h"

namespace GUI::Renderer
{
	class Image
	{
	public:
		Image(uint32_t width, uint32_t height);

		void Resize(uint32_t width, uint32_t height);

		const Texture& GetTexture() const;

		uint32_t GetWidth() const;
		uint32_t GetHeight() const;

	private:
		Texture m_Texture;
	};
}

