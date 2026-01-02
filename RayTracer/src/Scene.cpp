#include "Scene.h"

namespace RayTracer
{
	std::vector<Triangle> Quad::GetTriangles() const
	{
		Vector3 upperLeft(Center + (-Width + Height) / 2);
		Vector3 bottomLeft(Center + (-Width - Height) / 2);
		Vector3 bottomRight(Center + (Width - Height) / 2);
		Vector3 upperRight(Center + (Width + Height) / 2);

		std::vector<Triangle> triangles;
		triangles.reserve(2);
		triangles.emplace_back(upperLeft, bottomLeft, bottomRight, MaterialIndex);
		triangles.emplace_back(bottomRight, upperRight, upperLeft, MaterialIndex);

		return triangles;
	}

	std::vector<Triangle> Block::GetTriangles() const
	{
		Quad frontFace = { Center - Depth / 2, Width, Height, MaterialIndex };
		Quad backFace = { Center + Depth / 2, -Width, Height, MaterialIndex };
		Quad leftFace = { Center - Width / 2, -Depth, Height, MaterialIndex };
		Quad rightFace = { Center + Width / 2, Depth, Height, MaterialIndex };
		Quad bottomFace = { Center - Height / 2, Width, -Depth, MaterialIndex };
		Quad topFace = { Center + Height / 2, Width, Depth, MaterialIndex };

		std::vector<Triangle> triangles;
		triangles.reserve(12);

		for (const Triangle& triangle : frontFace.GetTriangles())
			triangles.push_back(triangle);
		for (const Triangle& triangle : backFace.GetTriangles())
			triangles.push_back(triangle);
		for (const Triangle& triangle : leftFace.GetTriangles())
			triangles.push_back(triangle);
		for (const Triangle& triangle : rightFace.GetTriangles())
			triangles.push_back(triangle);
		for (const Triangle& triangle : bottomFace.GetTriangles())
			triangles.push_back(triangle);
		for (const Triangle& triangle : topFace.GetTriangles())
			triangles.push_back(triangle);

		return triangles;
	}

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

	void Scene::AddBlock(
		const Vector3& position,
		const Vector3& width,
		const Vector3& height,
		const Vector3& depth,
		uint32_t materialIndex
	)
	{
		Block block = { position, width, height, depth, materialIndex };

		Mesh mesh;
		mesh.Triangles.reserve(12);
		for (const Triangle& triangle : block.GetTriangles())
			mesh.Triangles.push_back(triangle);

		AddMesh(mesh);
	}

	void Scene::AddMesh(const Mesh& mesh)
	{
		m_Meshes.push_back(mesh);
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

	const std::vector<Mesh>& Scene::GetMeshes() const
	{
		return m_Meshes;
	}

	std::vector<Mesh>& Scene::GetMeshes()
	{
		return m_Meshes;
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