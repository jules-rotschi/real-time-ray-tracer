#pragma once

#include "Vector3.h"

namespace RayTracer
{
	class Ray
	{
	public:
		Ray() = default;
		Ray(const Vector3& origin, const Vector3& direction);

		const Vector3& GetOrigin() const;
		const Vector3& GetDirection() const;

		Vector3 At(double t) const;

		double GetCurrentMediumRefractiveIndex() const;
		void SetCurrentMediumRefractiveIndex(double i);

	private:
		Vector3 m_Origin;
		Vector3 m_Direction;

		double m_CurrentMediumRefractiveIndex = 1.0;

		friend class Renderer;
	};
}
