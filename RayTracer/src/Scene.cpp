#include "Scene.h"

namespace RayTracer
{
	Scene::Scene(const Vector3& skyColor)
		: m_SkyColor(skyColor) {}

	void Scene::AddMaterial(const Material& material)
	{
		return m_Materials.push_back(material.GetUniquePtr());
	}

	void Scene::AddObject(const Object& object)
	{
		return m_Objects.push_back(object.GetUniquePtr());
	}

	const std::unique_ptr<Material>& Scene::GetMaterial(uint32_t index) const
	{
		return m_Materials[index];
	}

	const std::vector<std::unique_ptr<Object>>& Scene::GetObjects() const
	{
		return m_Objects;
	}

	uint32_t Scene::GetMaterialsCount() const
	{
		return m_Materials.size();
	}

	Vector3 Scene::GetSkyColor() const
	{
		return m_SkyColor;
	}

	Vector3& Scene::r_GetSkyColor()
	{
		return m_SkyColor;
	}
}