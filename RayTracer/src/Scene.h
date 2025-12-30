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

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
	};

	struct Triangle
	{
		Vertex V1;
		Vertex V2;
		Vertex V3;
		uint32_t MaterialIndex = 0;
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
		void AddTriangle(const Triangle& triangle);

		const std::vector<Material>& GetMaterials() const;
		std::vector<Material>& GetMaterials();
		const std::vector<Sphere>& GetSpheres() const;
		std::vector<Sphere>& GetSpheres();

		Vector3 GetSkyColor() const;
		Vector3& r_GetSkyColor();

	private:
		Vector3 m_SkyColor;

		std::vector<Material> m_Materials;
		std::vector<Sphere> m_Spheres;
		std::vector<Triangle> m_Triangles;

		friend class ShaderDataManager;
	};
}