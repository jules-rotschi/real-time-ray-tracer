#pragma once

#include <vector>
#include <memory>

#include "Vector3.h"
#include "Object.h"
#include "Material.h"

namespace RayTracer
{
	class Scene
	{
	public:
		Scene(const Vector3& skyColor = { 0.0, 0.0, 0.0 });

		void AddMaterial(const Material& material);
		void AddObject(const Object& object);

		const std::unique_ptr<Material>& GetMaterial(uint32_t index) const;
		const std::vector<std::unique_ptr<Object>>& GetObjects() const;

		uint32_t GetMaterialsCount() const;

		Vector3 GetSkyColor() const;
		Vector3& r_GetSkyColor();

	private:
		Vector3 m_SkyColor;

		std::vector<std::unique_ptr<Material>> m_Materials;
		std::vector<std::unique_ptr<Object>> m_Objects;
	};
}