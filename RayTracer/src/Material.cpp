#include "Material.h"

#include "Random.h"
#include "Ray.h"
#include "Hit.h"

namespace RayTracer
{
	Material::Material(const Vector3& albedo, double roughness, const Vector3& emittedColor)
		: m_Albedo(albedo), m_EmittedColor(emittedColor), m_Roughness(roughness) {}

	Vector3 Material::GetAlbedo() const
	{
		return m_Albedo;
	}

	Vector3 Material::GetEmittedColor() const
	{
		return m_EmittedColor;
	}

	double Material::GetRoughness() const
	{
		return m_Roughness;
	}

	Vector3& Material::r_GetAlbedo()
	{
		return m_Albedo;
	}

	Vector3& Material::r_GetEmittedColor()
	{
		return m_EmittedColor;
	}

	double& Material::r_GetRoughness()
	{
		return m_Roughness;
	}

	Opaque::Opaque(const Vector3& albedo, double roughness, double shine)
		: Material(albedo, roughness), m_Shine(shine) {}

	Opaque::Opaque(const Vector3& albedo, const Vector3& emittedColor)
		: Material(albedo, 1, emittedColor) {}

	bool Opaque::Scatter(const Ray& incidentRay, const Hit& hit, Ray& scatteredRay, uint32_t& seed) const
	{
		Vector3 scatteredDirection;

		if (m_Shine > 0 && (m_Shine >= 1 || Random::Double(seed) < m_Shine))
		{
			Vector3 reflectedDirection = incidentRay.GetDirection().Reflect(hit.UnitNormal);
			scatteredDirection = reflectedDirection.GetUnitVector() + GetRoughness() * Random::VectorInUnitSphere(seed);

			if (scatteredDirection.IsNearlyNull())
			{
				scatteredDirection = reflectedDirection;
			}
		}
		else
		{
			scatteredDirection = hit.UnitNormal + Random::VectorInUnitSphere(seed);
			if (scatteredDirection.IsNearlyNull()) {
				scatteredDirection = hit.UnitNormal;
			}
		}

		scatteredRay = Ray(hit.Position, scatteredDirection);
		return true;
	}

	double Opaque::GetShine() const
	{
		return m_Shine;
	}

	double& Opaque::r_GetShine()
	{
		return m_Shine;
	}

	std::unique_ptr<Material> Opaque::GetUniquePtr() const
	{
		return std::make_unique<Opaque>(*this);
	}

	Emissive::Emissive(const Vector3& albedo, const Vector3& emittedColor, double lightness)
		: Opaque(albedo, emittedColor * lightness) {}

	std::unique_ptr<Material> Emissive::GetUniquePtr() const
	{
		return std::make_unique<Emissive>(*this);
	}

	Clear::Clear(double refractiveIndex, const Vector3& albedo, double roughness)
		: Material(albedo, roughness), m_RefractiveIndex(refractiveIndex) {}

	bool Clear::Scatter(const Ray& incidentRay, const Hit& hit, Ray& scatteredRay, uint32_t& seed) const
	{
		double firstMediumIndex = incidentRay.GetCurrentMediumRefractiveIndex();
		double secondMediumIndex = hit.FrontFace ? m_RefractiveIndex : 1;
		double refractiveIndicesRatio = firstMediumIndex / secondMediumIndex;

		Vector3 incidentUnitVector = incidentRay.GetDirection().GetUnitVector();

		double cosIncidentAngle = Dot(incidentUnitVector, -hit.UnitNormal);

		bool doesRayOnlyReflect = refractiveIndicesRatio * std::sqrt(1 - cosIncidentAngle * cosIncidentAngle) > 1;

		double reflectance = (1 - secondMediumIndex) / (1 + secondMediumIndex);
		double squaredReflectance = reflectance * reflectance;
		double reflexionProbability =
			squaredReflectance
			+ (1 - squaredReflectance)
			* std::pow(1 - cosIncidentAngle, 5);

		Vector3 scatteredDirection;

		if (doesRayOnlyReflect || Random::Double(seed) < reflexionProbability)
		{
			Vector3 reflectedDirection = incidentUnitVector.Reflect(hit.UnitNormal);
			scatteredDirection = reflectedDirection + GetRoughness() * Random::VectorInUnitSphere(seed);
		}
		else {
			Vector3 refracted_direction = incidentUnitVector.Refract(hit.UnitNormal, refractiveIndicesRatio);
			scatteredDirection = refracted_direction + GetRoughness() * Random::VectorInUnitSphere(seed);
		}

		scatteredRay = Ray(hit.Position, scatteredDirection);
		scatteredRay.SetCurrentMediumRefractiveIndex(secondMediumIndex);

		return true;
	}

	double Clear::GetRefractiveIndex() const
	{
		return m_RefractiveIndex;
	}

	double& Clear::r_GetRefractiveIndex()
	{
		return m_RefractiveIndex;
	}

	std::unique_ptr<Material> Clear::GetUniquePtr() const
	{
		return std::make_unique<Clear>(*this);
	}
}