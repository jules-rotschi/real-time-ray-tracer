#pragma once

#include <stdint.h>

#include "Vector3.h"

namespace RayTracer
{
	class Material;
	class Ray;

	struct Hit
	{
		double T = -1.0;
		Vector3 Position;
		Vector3 UnitNormal;
		bool FrontFace = false;
		uint32_t MaterialIndex = 0;

		void SetFaceNormal(const Ray& ray, const Vector3& outwardNormal);
	};
}
