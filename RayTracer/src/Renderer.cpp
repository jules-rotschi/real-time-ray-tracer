#include "Renderer.h"

#include <cmath>
#include <numbers>
#include <limits>
#include <thread>
#include <execution>
#include <algorithm>

#include "Vector3.h"
#include "Interval.h"
#include "Random.h"
#include "Scene.h"
#include "Material.h"
#include "Object.h"
#include "Ray.h"
#include "Hit.h"
#include "Camera.h"

namespace RayTracer
{
	void Renderer::Render(const Scene& scene, const Camera& camera)
	{
		m_Scene = &scene;
		m_Camera = &camera;

		std::for_each(std::execution::par, m_VerticalIterator.begin(), m_VerticalIterator.end(), [this](uint32_t y)
		{
			std::for_each(std::execution::par, m_HorizontalIterator.begin(), m_HorizontalIterator.end(), [this, y](uint32_t x)
			{
				Vector3 pixelValue;

				if (m_Settings.Preview)
					pixelValue = GetPreviewPixel(x, y);
				else
					pixelValue = GetRayTracedPixel(x, y);

				if (m_Settings.FocusPeaking && IsPixelOnFocusSphere(x, y))
					pixelValue = { 1.0, 0.0, 0.0 };

				uint32_t rIndex = 4 * (x + y * m_Image.GetWidth());
				uint32_t gIndex = rIndex + 1;
				uint32_t bIndex = gIndex + 1;
				uint32_t aIndex = bIndex + 1;

				m_ImageData[rIndex] = static_cast<uint8_t>(pixelValue.R * 255);
				m_ImageData[gIndex] = static_cast<uint8_t>(pixelValue.G * 255);
				m_ImageData[bIndex] = static_cast<uint8_t>(pixelValue.B * 255);
				m_ImageData[aIndex] = 255;
			});
		});

		m_Image.SetData(m_ImageData);

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
			m_AccumulationTime += dt;
		}
	}

	void Renderer::OnResize(uint32_t width, uint32_t height)
	{
		// No resize necessary
		if (m_Image.GetWidth() == width && m_Image.GetHeight() == height)
			return;

		m_Image.Resize(width, height);

		delete[] m_ImageData;
		m_ImageData = new uint8_t[width * height * 4];

		delete[] m_LuminanceIntegrationData;
		m_LuminanceIntegrationData = new Vector3[width * height];

		m_HorizontalIterator.resize(width);
		m_VerticalIterator.resize(height);
		for (uint32_t i = 0; i < width; i++)
			m_HorizontalIterator[i] = i;
		for (uint32_t i = 0; i < height; i++)
			m_VerticalIterator[i] = i;

		ResetFrameIndex();
	}

	uint8_t* Renderer::GetImageData() const
	{
		return m_ImageData;
	}

	GUI::Image Renderer::GetImage() const
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

	double Renderer::GetAccumulationTime() const
	{
		return m_AccumulationTime;
	}

	Vector3 Renderer::GetRayTracedPixel(uint32_t x, uint32_t y)
	{
		uint32_t seed = x + y * m_Image.GetWidth();
		seed *= m_FrameIndex;

		Vector3 instantLuminance = ComputePixelLuminance(x, y, seed);

		Vector3& integralLuminance = m_LuminanceIntegrationData[x + y * m_Image.GetWidth()];

		if (m_FrameIndex == 1)
			integralLuminance = Vector3(0, 0, 0);

		integralLuminance += instantLuminance;

		Vector3 continuousLuminance = integralLuminance / m_FrameIndex;
		return m_Camera->GetPixel(continuousLuminance);
	}

	Vector3 Renderer::GetPreviewPixel(uint32_t x, uint32_t y) const
	{
		Vector3 rayOrigin = m_Camera->GetPosition();
		Vector3 rayDirection = (m_Camera->GetVirtualPixelPosition(x, y) - rayOrigin).GetUnitVector();
		Ray ray(rayOrigin, rayDirection);

		Hit hit = TraceRay(ray);

		if (hit.T <= 0)
		{
			switch (m_Settings.PreviewMode)
			{
				using enum PreviewMode;
			case DEPTH:
				return { 0.0, 0.0, 0.0 };
			case SHADE:
				return m_Camera->GetPixel(m_Scene->GetSkyColor());
			}
		}

		if (m_Settings.PreviewMode == PreviewMode::DEPTH)
		{
			double depthInvert = hit.T < 1.0 ? 1.0 : 1.0 / hit.T;
			return { depthInvert, depthInvert, depthInvert };
		}

		Vector3 illumination;

		for (const std::unique_ptr<Object>& object : m_Scene->GetObjects())
		{
			const Emissive* light = dynamic_cast<Emissive*>(m_Scene->GetMaterial(object->GetMaterialIndex()).get());
			if (!light) continue;

			Vector3 hitToLight = object->GetCenter() - hit.Position;

			double cosAngle = Dot(hit.UnitNormal, hitToLight.GetUnitVector());

			illumination += light->GetEmittedColor() / 2 * object->GetSize() * cosAngle / hitToLight.GetSquaredLength();
		}

		Vector3 luminance = MultiplyColors(illumination, m_Scene->GetMaterial(hit.MaterialIndex)->GetAlbedo() / std::numbers::pi);

		return m_Camera->GetPixel(luminance);
	}

	bool Renderer::IsPixelOnFocusSphere(uint32_t x, uint32_t y) const
	{
		Vector3 rayOrigin = m_Camera->GetPosition();
		Vector3 rayDirection = (m_Camera->GetVirtualPixelPosition(x, y) - rayOrigin).GetUnitVector();
		Ray ray(rayOrigin, rayDirection);

		Hit hit = TraceRay(ray);

		return std::abs(hit.T - m_Camera->GetLens().FocusDistance) <= 0.01;
	}

	Vector3 Renderer::ComputePixelLuminance(uint32_t x, uint32_t y, uint32_t& seed) const
	{
		Vector3 rayOrigin = m_Camera->GetRayOrigin(m_Settings.DepthOfField, seed);
		Vector3 rayDirection = (m_Camera->GetVirtualPixelPosition(x, y, m_Settings.Antialiasing, seed) - rayOrigin).GetUnitVector();
		Ray ray(rayOrigin, rayDirection);

		Hit hit = TraceRay(ray);
		
		if (hit.T <= 0)
			return m_Scene->GetSkyColor();

		Vector3 illumination = ComputeIllumination(ray, hit, m_Settings.Depth, seed);
		Vector3 luminance = MultiplyColors(illumination, m_Scene->GetMaterial(hit.MaterialIndex)->GetAlbedo() / std::numbers::pi);

		return luminance;
	}

	Hit Renderer::TraceRay(const Ray& ray) const
	{
		double tMin = 1e-3;

		Hit hit;
		double closest = std::numeric_limits<double>::infinity();

		for (const std::unique_ptr<Object>& object : m_Scene->GetObjects())
		{
			if (object->RayIntersection(ray, Interval(tMin, closest), hit))
			{
				closest = hit.T;
			}
		}

		return hit;
	}

	Vector3 Renderer::ComputeIllumination(Ray incidentRay, Hit hit, uint32_t depth, uint32_t& seed) const
	{
		Vector3 illumination;
		Vector3 contribution = { 1.0, 1.0, 1.0 };

		for (uint32_t i = 0; i <= depth; i++)
		{
			Vector3 ownLight = m_Scene->GetMaterial(hit.MaterialIndex)->GetEmittedColor() * std::numbers::pi;

			if (i == depth)
			{
				illumination += MultiplyColors(ownLight, contribution);
				break;
			}

			Ray scatteredRay;
			m_Scene->GetMaterial(hit.MaterialIndex)->Scatter(incidentRay, hit, scatteredRay, seed);
			Hit nextHit = TraceRay(scatteredRay);

			if (nextHit.T == -1)
			{
				illumination += MultiplyColors(ownLight + m_Scene->GetSkyColor() * std::numbers::pi, contribution);
				break;
			}

			Vector3 directIllumination = m_Scene->GetMaterial(nextHit.MaterialIndex)->GetEmittedColor();

			seed++;

			illumination += MultiplyColors(ownLight + directIllumination, contribution);

			contribution = MultiplyColors(contribution, m_Scene->GetMaterial(nextHit.MaterialIndex)->GetAlbedo());

			incidentRay = scatteredRay;
			hit = nextHit;
		}

		return illumination;
	}

	Vector3 Renderer::MultiplyColors(Vector3 color, Vector3 albedo) const
	{
		return {
			color.R * albedo.R,
			color.G * albedo.G,
			color.B * albedo.B
		};
	}
}