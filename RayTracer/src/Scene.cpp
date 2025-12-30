#include "Scene.h"

namespace RayTracer
{
	Scene::Scene(const Vector3& skyColor)
		: m_SkyColor(skyColor) {}

	uint32_t Scene::AddMaterial(const Material& material)
	{
		m_Materials.push_back(material);
		return static_cast<uint32_t>(m_Materials.size() - 1);
	}

	void Scene::AddSphere(const Sphere& sphere)
	{
		m_Spheres.push_back(sphere);
	}

	void Scene::AddTriangle(const Triangle& triangle)
	{
		m_Triangles.push_back(triangle);
	}

	const std::vector<Material>& Scene::GetMaterials() const
	{
		return m_Materials;
	}

	std::vector<Material>& Scene::GetMaterials()
	{
		return m_Materials;
	}

	const std::vector<Sphere>& Scene::GetSpheres() const
	{
		return m_Spheres;
	}

	std::vector<Sphere>& Scene::GetSpheres()
	{
		return m_Spheres;
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