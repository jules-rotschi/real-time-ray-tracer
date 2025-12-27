#pragma once

#include "Vector3.h"
#include "Object.h"

namespace RayTracer
{
	class Sphere : public Object
	{
	public:
		Sphere(const Vector3& position, double radius, uint32_t materialIndex);

		bool RayIntersection(const Ray& ray, Interval tInterval, Hit& hit) const override;

		const Vector3& GetPosition() const;
		Vector3& r_GetPosition();

		double GetRadius() const;
		double& r_GetRadius();

		Vector3 GetCenter() const override;
		double GetSize() const override;

		std::unique_ptr<Object> GetUniquePtr() const override;
		
	private:
		Vector3 m_Position;
		double m_Radius;
	};
}

