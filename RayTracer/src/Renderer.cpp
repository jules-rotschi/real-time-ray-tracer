#include "Renderer.h"

#include "Vector3.h"
#include "Scene.h"
#include "Camera.h"

namespace RayTracer
{
	Renderer::Renderer()
		: m_RayTracingShader("RayTracer/shaders/RayTracingShader.glsl"),
		  m_CameraShader("RayTracer/shaders/CameraShader.glsl"),
		  m_FinalImage(0, 0) {}

	void Renderer::Render(const Scene& scene, const Camera& camera)
	{
		m_Scene = &scene;
		m_Camera = &camera;

		m_RayTracingShader.Use();

		glBindImageTexture(0, m_RayTracedTexture.GetHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		m_ShaderDataManager.UpdateData(*this);

		m_UniformBuffer.BindTo(1);
		m_UniformBuffer.SetData(m_ShaderDataManager.GetData());

		m_VirtualPixelsBuffer.BindTo(2);
		m_IntegratedLuminanceBuffer.BindTo(3);

		const GLuint workGroupSizeX = 16;
		const GLuint workGroupSizeY = 16;

		GLuint numGroupsX = (m_FinalImage.GetWidth() + workGroupSizeX - 1) / workGroupSizeX;
		GLuint numGroupsY = (m_FinalImage.GetHeight() + workGroupSizeY - 1) / workGroupSizeY;

		m_RayTracingShader.Compute(numGroupsX, numGroupsY, 1, GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		if (m_Settings.Bloom)
			m_BloomRenderer.Render(m_RayTracedTexture, 0.005f);

		m_CameraShader.Use();

		if (m_Settings.Bloom)
			glBindImageTexture(0, m_BloomRenderer.GetFinalTexture().GetHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		else
			glBindImageTexture(0, m_RayTracedTexture.GetHandle(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

		glBindImageTexture(1, m_FinalImage.GetTexture().GetHandle(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		glUniform1f(0, camera.GetExposure());
		glUniform1f(1, camera.GetSensor().Gamma);

		m_CameraShader.Compute(numGroupsX, numGroupsY, 1, GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Focus peaking shader

		if (!m_Settings.Preview)
		{
			if (m_Settings.Accumulation)
				m_FrameIndex++;
			else
				m_FrameIndex = 1;
		}
	}

	void Renderer::OnUpdate(double dt)
	{
		if (m_Settings.Accumulation && !m_Settings.Preview)
		{
			m_AccumulationTime += static_cast<float>(dt);
		}
	}

	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		// No resize necessary
		if (m_FinalImage.GetWidth() == width && m_FinalImage.GetHeight() == height)
			return;

		m_RayTracedTexture.Resize(width, height);
		m_BloomRenderer.OnResize(width, height);
		m_FinalImage.Resize(width, height);

		m_VirtualPixelsBuffer.Resize(16 + static_cast<size_t>(width * height) * sizeof(Vec4));
		m_IntegratedLuminanceBuffer.Resize(static_cast<size_t>(width * height) * sizeof(Vec4));

		ResetFrameIndex();
	}

	const GUI::Renderer::Image& Renderer::GetFinalImage() const
	{
		return m_FinalImage;
	}

	Renderer::Settings& Renderer::GetSettings()
	{
		return m_Settings;
	}

	void Renderer::ResetFrameIndex()
	{
		m_FrameIndex = 1;
		m_AccumulationTime = 0.0;
	}

	float Renderer::GetAccumulationTime() const
	{
		return m_AccumulationTime;
	}
}