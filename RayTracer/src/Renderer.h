#pragma once

#include <vector>

#include "renderer/Image.h"
#include "renderer/Shader.h"
#include "renderer/UniformBuffer.h"
#include "renderer/ShaderStorageBuffer.h"

#include "ShaderData.h"
#include "BloomRenderer.h"

namespace RayTracer
{
	struct Vector3;

	class Scene;
	class Camera;

	class Renderer
	{
	public:
		enum class PreviewMode
		{
			DEPTH = 1,
			SHADE = 2
		};

		struct Settings
		{
			bool Preview = false;
			PreviewMode PreviewMode = PreviewMode::DEPTH;
			bool Accumulation = true;
			bool Antialiasing = true;
			bool DepthOfField = true;
			bool FocusPeaking = false;
			uint32_t Depth = 10;
			uint32_t Rays = 1;
			bool Bloom = false;
		};

	public:
		Renderer();

		void Render(const Scene& scene, const Camera& camera);

		void OnUpdate(double dt);
		void OnResize(uint32_t width, uint32_t height);

		const GUI::Renderer::Image& GetFinalImage() const;

		Settings& GetSettings();

		void ResetFrameIndex();
		float GetAccumulationTime() const;

	private:
		Settings m_Settings;

		const Scene* m_Scene = nullptr;
		const Camera* m_Camera = nullptr;

		uint32_t m_FrameIndex = 1;
		float m_AccumulationTime = 0.0;

		GUI::Renderer::Texture m_RayTracedTexture;

		GUI::Renderer::Shader m_RayTracingShader;
		GUI::Renderer::Shader m_CameraShader;
		
		BloomRenderer m_BloomRenderer;
		
		GUI::Renderer::Image m_FinalImage;

		ShaderDataManager m_ShaderDataManager;
		GUI::Renderer::UniformBuffer<ShaderData> m_UniformBuffer;
		GUI::Renderer::ShaderStorageBuffer<VirtualPixelsData> m_VirtualPixelsBuffer;
		GUI::Renderer::ShaderStorageBuffer<IntegratedLuminanceData> m_IntegratedLuminanceBuffer;

		friend class ShaderDataManager;
	};
}