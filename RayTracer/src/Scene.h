#pragma once

#include <vector>

#include "Vector3.h"

namespace RayTracer
{
	struct Material
	{
		bool IsOpaque = true;
		Vector3 Albedo;
		Vector3 EmittedColor;
		float Roughness = 0.0f;
		float Shine = 0.0f;
		float RefractiveIndex = 1.0f;
	};

	struct Triangle
	{
		Vector3 V0;
		Vector3 V1;
		Vector3 V2;
		uint32_t MaterialIndex = 0;
	};

	struct Quad
	{
		const Vector3& Center;
		const Vector3& Width;
		const Vector3& Height;
		uint32_t MaterialIndex = 0;

		std::vector<Triangle> GetTriangles() const;
	};

	struct Block
	{
		const Vector3& Center;
		const Vector3& Width;
		const Vector3& Height;
		const Vector3& Depth;
		uint32_t MaterialIndex = 0;

		std::vector<Triangle> GetTriangles() const;
	};

	struct Mesh
	{
		std::vector<Triangle> Triangles;
	};

	struct Sphere
	{
		Vector3 Position;
		float Radius = 1.0f;
		uint32_t MaterialIndex = 0;
	};

	class Scene
	{
	public:
		Scene(const Vector3& skyColor = { 0.0, 0.0, 0.0 });

		uint32_t AddMaterial(const Material& material);
		void AddSphere(const Sphere& sphere);
		void AddBlock(const Vector3& position, const Vector3& width, const Vector3& height, const Vector3& depth, uint32_t materialIndex);

		void AddMesh(const Mesh& mesh);

		const std::vector<Material>& GetMaterials() const;
		std::vector<Material>& GetMaterials();

		const std::vector<Sphere>& GetSpheres() const;
		std::vector<Sphere>& GetSpheres();

		const std::vector<Mesh>& GetMeshes() const;
		std::vector<Mesh>& GetMeshes();

		Vector3 GetSkyColor() const;
		Vector3& r_GetSkyColor();

	private:
		Vector3 m_SkyColor;

		std::vector<Material> m_Materials;
		std::vector<Sphere> m_Spheres;
		std::vector<Mesh> m_Meshes;

		friend class ShaderDataManager;
	};
}