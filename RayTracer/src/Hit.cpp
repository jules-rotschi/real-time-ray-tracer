#include "Hit.h"

#include "Ray.h"

namespace RayTracer
{
	void Hit::SetFaceNormal(const Ray& ray, const Vector3& outwardNormal)
	{
		FrontFace = Dot(ray.GetDirection(), outwardNormal) < 0;
		UnitNormal = FrontFace ? outwardNormal : -outwardNormal;
	}
}