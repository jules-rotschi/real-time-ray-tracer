#include "Ray.h"

namespace RayTracer
{
	Ray::Ray(const Vector3& origin, const Vector3& direction)
		: m_Origin(origin), m_Direction(direction) {}

	const Vector3& Ray::GetOrigin() const
	{
		return m_Origin;
	}

	const Vector3& Ray::GetDirection() const
	{
		return m_Direction;
	}

	Vector3 Ray::At(double t) const
	{
		return m_Origin + t * m_Direction;
	}

	double Ray::GetCurrentMediumRefractiveIndex() const
	{
		return m_CurrentMediumRefractiveIndex;
	}

	void Ray::SetCurrentMediumRefractiveIndex(double i)
	{
		m_CurrentMediumRefractiveIndex = i;
	}
}