#pragma once

#include <memory>

#include "Vector3.h"

namespace RayTracer
{
	class Ray;
	struct Hit;

	class Material
	{
	public:
		Material(const Vector3& albedo, double roughness, const Vector3& emittedColor = Vector3(0, 0, 0));

		virtual bool Scatter(const Ray& incidentRay, const Hit& hit, Ray& scatteredRay, uint32_t& seed) const = 0;

		Vector3 GetAlbedo() const;
		Vector3 GetEmittedColor() const;
		double GetRoughness() const;

		Vector3& r_GetAlbedo();
		Vector3& r_GetEmittedColor();
		double& r_GetRoughness();

		virtual std::unique_ptr<Material> GetUniquePtr() const = 0;

	private:
		Vector3 m_Albedo;
		Vector3 m_EmittedColor;
		double m_Roughness = 0.0;
	};

	class Opaque : public Material
	{
	public:
		Opaque(const Vector3& albedo, double roughness, double shine);
		Opaque(const Vector3& albedo, const Vector3& emittedColor);

		bool Scatter(const Ray& incidentRay, const Hit& hit, Ray& scatteredRay, uint32_t& seed) const override;

		double GetShine() const;
		double& r_GetShine();

		std::unique_ptr<Material> GetUniquePtr() const override;

	private:
		double m_Shine = 0;
	};

	class Emissive : public Opaque
	{
	public:
		Emissive(const Vector3& albedo, const Vector3& emittedColor, double lightness);

		std::unique_ptr<Material> GetUniquePtr() const override;
	};

	class Clear : public Material
	{
	public:
		Clear(double refractiveIndex, const Vector3& albedo, double roughness);

		bool Scatter(const Ray& incidentRay, const Hit& hit, Ray& scatteredRay, uint32_t& seed) const override;

		double GetRefractiveIndex() const;
		double& r_GetRefractiveIndex();
		
		std::unique_ptr<Material> GetUniquePtr() const override;

	private:
		double m_RefractiveIndex;
	};
}
