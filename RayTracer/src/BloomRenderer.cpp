#include "BloomRenderer.h"

namespace RayTracer
{
	BloomRenderer::BloomRenderer()
		: m_FinalTexture(0, 0, GUI::Renderer::Texture::FilteringMode::LINEAR),
		m_DownsampleShader("RayTracer/shaders/BloomDownsamplingShader.glsl"),
		m_UpsampleShader("RayTracer/shaders/BloomUpsamplingShader.glsl"),
		m_BlendingShader("RayTracer/shaders/BloomBlendingShader.glsl")
	{
		m_BloomBuffer.Init(m_ViewportWidth, m_ViewportHeight, 5);
	}

	void BloomRenderer::Render(const GUI::Renderer::Texture& sourceTexture, float filterRadius) const
	{
		RenderDownsamples(sourceTexture);
		RenderUpsamples(filterRadius);
		Blend(sourceTexture);
	}

	void BloomRenderer::OnResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_BloomBuffer.Resize(width, height);
		m_FinalTexture.Resize(width, height);
	}

	const GUI::Renderer::Texture& BloomRenderer::GetFinalTexture() const
	{
		return m_FinalTexture;
	}

	void BloomRenderer::RenderDownsamples(const GUI::Renderer::Texture& sourceTexture) const
	{
		const std::vector<GUI::Renderer::Texture>& mipChain = m_BloomBuffer.GetMipChain();

		m_DownsampleShader.Use();

		glBindImageTexture(0, sourceTexture.GetHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		//glBindTextureUnit(0, sourceTexture.GetHandle());

		const GLuint workGroupWidth = 16;
		const GLuint workGroupHeight = 16;

		for (const GUI::Renderer::Texture& mip : mipChain)
		{
			glBindImageTexture(1, mip.GetHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

			GLuint numGroupsX = (mip.GetWidth() + workGroupWidth - 1) / workGroupWidth;
			GLuint numGroupsY = (mip.GetHeight() + workGroupHeight - 1) / workGroupHeight;

			m_DownsampleShader.Compute(numGroupsX, numGroupsY, 1, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

			glBindImageTexture(0, mip.GetHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			//glBindTextureUnit(0, mip.GetHandle());
		}
	}

	void BloomRenderer::RenderUpsamples(float filterRadius) const
	{
		const std::vector<GUI::Renderer::Texture>& mipChain = m_BloomBuffer.GetMipChain();

		m_UpsampleShader.Use();
		glUniform1f(0, filterRadius);

		const GLuint workGroupWidth = 16;
		const GLuint workGroupHeight = 16;

		for (size_t i = mipChain.size() - 1; i > 0; i--)
		{
			const GUI::Renderer::Texture& mip = mipChain[i];
			const GUI::Renderer::Texture& nextMip = mipChain[i - 1];

			glBindImageTexture(0, mip.GetHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
			glBindImageTexture(1, nextMip.GetHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

			GLuint numGroupsX = (nextMip.GetWidth() + workGroupWidth - 1) / workGroupWidth;
			GLuint numGroupsY = (nextMip.GetHeight() + workGroupHeight - 1) / workGroupHeight;

			m_UpsampleShader.Compute(numGroupsX, numGroupsY, 1, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}
	}

	void BloomRenderer::Blend(const GUI::Renderer::Texture& sourceTexture) const
	{
		m_BlendingShader.Use();

		const GLuint workGroupWidth = 16;
		const GLuint workGroupHeight = 16;

		GLuint numGroupsX = (m_FinalTexture.GetWidth() + workGroupWidth - 1) / workGroupWidth;
		GLuint numGroupsY = (m_FinalTexture.GetHeight() + workGroupHeight - 1) / workGroupHeight;

		glBindImageTexture(0, sourceTexture.GetHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, m_BloomBuffer.GetMipChain()[0].GetHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(2, m_FinalTexture.GetHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		m_BlendingShader.Compute(numGroupsX, numGroupsY, 1, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}
}