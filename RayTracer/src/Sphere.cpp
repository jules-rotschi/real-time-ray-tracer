#include "Sphere.h"

#include <cmath>
#include <numbers>

#include "Interval.h"
#include "Material.h"
#include "Ray.h"
#include "Hit.h"

namespace RayTracer
{
	Sphere::Sphere(const Vector3& position, double radius, uint32_t materialIndex)
		: Object(materialIndex), m_Position(position), m_Radius(radius) {}

	bool Sphere::RayIntersection(const Ray& ray, Interval tInterval, Hit& hit) const
	{
		Vector3 rayDirection = ray.GetDirection();
		Vector3 sphereDirection = m_Position - ray.GetOrigin();

		double a = rayDirection.GetSquaredLength();
		double b = -2 * Dot(rayDirection, sphereDirection);
		double c = sphereDirection.GetSquaredLength() - m_Radius * m_Radius;
		double discriminant = b * b - 4 * a * c;

		if (discriminant < 0)
			return false;

		double discriminant_sqrt = std::sqrt(discriminant);

		double t = (-b - discriminant_sqrt) / (2 * a);
		if (!tInterval.DoesContain(t))
		{
			t = (-b + discriminant_sqrt) / (2 * a);
			if (!tInterval.DoesContain(t)) return false;
		}
		
		hit.T = t;
		hit.Position = ray.At(t);
		Vector3 outwardNormal = (hit.Position - m_Position) / m_Radius;
		hit.SetFaceNormal(ray, outwardNormal);
		hit.MaterialIndex = GetMaterialIndex();

		return true;
	}

	const Vector3& Sphere::GetPosition() const
	{
		return m_Position;
	}

	Vector3& Sphere::r_GetPosition()
	{
		return m_Position;
	}

	double Sphere::GetRadius() const
	{
		return m_Radius;
	}

	double& Sphere::r_GetRadius()
	{
		return m_Radius;
	}

	Vector3 Sphere::GetCenter() const
	{
		return m_Position;
	}

	double Sphere::GetSize() const
	{
		return std::numbers::pi * m_Radius * m_Radius;
	}

	std::unique_ptr<Object> Sphere::GetUniquePtr() const
	{
		return std::make_unique<Sphere>(*this);
	}
}