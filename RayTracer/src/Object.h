#pragma once

#include <memory>

namespace RayTracer
{
	class Interval;
	struct Vector3;

	class Material;
	
	class Ray;
	struct Hit;

	class Object
	{
	public:
		Object(uint32_t materialIndex);
		
		virtual bool RayIntersection(const Ray& ray, Interval tInterval, Hit& hit) const = 0;

		virtual std::unique_ptr<Object> GetUniquePtr() const = 0;
	
		uint32_t GetMaterialIndex() const;
		uint32_t& r_GetMaterialIndex();

		virtual Vector3 GetCenter() const = 0;
		virtual double GetSize() const = 0;

	private:
		uint32_t m_MaterialIndex;
	};
}
