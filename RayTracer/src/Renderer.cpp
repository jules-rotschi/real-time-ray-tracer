#include "Renderer.h"

#include "Vector3.h"
#include "Scene.h"
#include "Camera.h"

namespace RayTracer
{
	Renderer::Renderer()
		: m_Image(0, 0), m_Shader("RayTracer/shaders/RayTracingShader.glsl") {}

	void Renderer::Render(const Scene& scene, const Camera& camera)
	{
		m_Scene = &scene;
		m_Camera = &camera;

		m_Shader.Use();
		glBindImageTexture(0, static_cast<GLuint>(m_Image.GetTexture()), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		m_ShaderDataManager.UpdateData(*this);

		m_UniformBuffer.BindTo(1);
		m_UniformBuffer.SetData(m_ShaderDataManager.GetData());

		m_VirtualPixelsBuffer.BindTo(2);
		m_IntegratedLuminanceBuffer.BindTo(3);

		const GLuint workGroupSizeX = 16;
		const GLuint workGroupSizeY = 16;

		GLuint numGroupsX = (m_Image.GetWidth() + workGroupSizeX - 1) / workGroupSizeX;
		GLuint numGroupsY = (m_Image.GetHeight() + workGroupSizeY - 1) / workGroupSizeY;

		glDispatchCompute(numGroupsX, numGroupsY, 1);

		//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

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
		if (m_Image.GetWidth() == width && m_Image.GetHeight() == height)
			return;

		m_Image.Resize(width, height);

		m_VirtualPixelsBuffer.Resize(16 + static_cast<size_t>(width * height) * sizeof(Vec4));
		m_IntegratedLuminanceBuffer.Resize(static_cast<size_t>(width * height) * sizeof(Vec4));

		ResetFrameIndex();
	}

	GUI::Renderer::Image Renderer::GetImage() const
	{
		return m_Image;
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