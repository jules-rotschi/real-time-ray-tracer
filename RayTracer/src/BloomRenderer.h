#pragma once

#include "renderer/Texture.h"
#include "renderer/Shader.h"

#include "BloomBuffer.h"

namespace RayTracer
{
	class BloomRenderer
	{
	public:
		BloomRenderer();

		void Render(const GUI::Renderer::Texture& sourceTexture, float filterRadius) const;

		void OnResize(uint32_t width, uint32_t height);

		const GUI::Renderer::Texture& GetFinalTexture() const;

	private:
		void RenderDownsamples(const GUI::Renderer::Texture& sourceTexture) const;
		void RenderUpsamples(float filterRadius) const;
		void Blend(const GUI::Renderer::Texture& sourceTexture) const;

	private:
		BloomBuffer m_BloomBuffer;
		GUI::Renderer::Texture m_FinalTexture;

		uint32_t m_ViewportWidth = 0;
		uint32_t m_ViewportHeight = 0;

		GUI::Renderer::Shader m_DownsampleShader;
		GUI::Renderer::Shader m_UpsampleShader;
		GUI::Renderer::Shader m_BlendingShader;
	};
}

