#pragma once

#include <vector>

#include "Image.h"

namespace RayTracer
{
	struct Vector3;

	class Scene;
	class Material;
	
	class Ray;
	struct Hit;
	
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
			int Depth = 5;
		};

	public:
		void Render(const Scene& scene, const Camera& camera);

		void OnUpdate(double dt);
		void OnResize(uint32_t width, uint32_t height);

		uint8_t* GetImageData() const;
		GUI::Image GetImage() const;

		Settings& GetSettings();

		void ResetFrameIndex();
		double GetAccumulationTime() const;

	private:
		Vector3 GetRayTracedPixel(uint32_t x, uint32_t y);
		Vector3 GetPreviewPixel(uint32_t x, uint32_t y) const;

		bool IsPixelOnFocusSphere(uint32_t x, uint32_t y) const;

		Vector3 ComputePixelLuminance(uint32_t x, uint32_t y, uint32_t& seed) const;
		Hit TraceRay(const Ray& ray) const;
		Vector3 ComputeIllumination(Ray incidentRay, Hit hit, uint32_t depth, uint32_t& seed) const;

		Vector3 MultiplyColors(Vector3 color, Vector3 albedo) const;

	private:
		Settings m_Settings;

		const Scene* m_Scene = nullptr;
		const Camera* m_Camera = nullptr;

		uint8_t* m_ImageData = nullptr;
		Vector3* m_LuminanceIntegrationData = nullptr;

		uint32_t m_FrameIndex = 1;
		double m_AccumulationTime = 0.0;

		std::vector<uint32_t> m_VerticalIterator;
		std::vector<uint32_t> m_HorizontalIterator;

		GUI::Image m_Image;
	};
}