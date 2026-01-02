#include "ShaderData.h"

#include <iostream>

#include "renderer/Shader.h"

#include "Renderer.h"
#include "Camera.h"
#include "Scene.h"

namespace RayTracer
{
	void ShaderDataManager::UpdateData(const Renderer& renderer)
	{
		m_Data.Settings.Preview = renderer.m_Settings.Preview;
		m_Data.Settings.PreviewMode = static_cast<uint32_t>(renderer.m_Settings.PreviewMode);
		m_Data.Settings.Accumulation = renderer.m_Settings.Accumulation;
		m_Data.Settings.Antialiasing = renderer.m_Settings.Antialiasing;
		m_Data.Settings.DepthOfField = renderer.m_Settings.DepthOfField;
		m_Data.Settings.FocusPeaking = renderer.m_Settings.FocusPeaking;
		m_Data.Settings.Depth = renderer.m_Settings.Depth;
		m_Data.Settings.Rays = renderer.m_Settings.Rays;
		m_Data.FrameIndex = renderer.m_FrameIndex;
		m_Data.InvalidPixelPositions = renderer.m_Camera->m_InvalidPixelPositions;

		m_Data.Camera.Position = renderer.m_Camera->m_Position;
		m_Data.Camera.E1 = renderer.m_Camera->m_Trihedron.E1;
		m_Data.Camera.E2 = renderer.m_Camera->m_Trihedron.E2;
		m_Data.Camera.E3 = renderer.m_Camera->m_Trihedron.E3;
		m_Data.Camera.SensorWidth = renderer.m_Camera->m_Sensor.Width;
		m_Data.Camera.SensorHeight = renderer.m_Camera->m_Sensor.Height;
		m_Data.Camera.Sensitivity = renderer.m_Camera->m_Sensor.Sensitivity;
		m_Data.Camera.Gamma = renderer.m_Camera->m_Sensor.Gamma;
		m_Data.Camera.FocalLength = renderer.m_Camera->m_Lens.FocalLength;
		m_Data.Camera.FocusDistance = renderer.m_Camera->m_Lens.FocusDistance;
		m_Data.Camera.Aperture = renderer.m_Camera->m_Lens.Aperture;
		m_Data.Camera.ShutterSpeed = renderer.m_Camera->m_ShutterSpeed;
		m_Data.Camera.NeutralDensityValue = renderer.m_Camera->m_NeutralDensityValue;

		m_Data.SkyColor = renderer.m_Scene->m_SkyColor;

		m_Data.MaterialCount = static_cast<uint32_t>(renderer.m_Scene->GetMaterials().size());

		for (uint32_t i = 0; i < renderer.m_Scene->GetMaterials().size(); i++)
		{
			m_Data.Materials[i].IsOpaque = renderer.m_Scene->GetMaterials()[i].IsOpaque;
			m_Data.Materials[i].Albedo = renderer.m_Scene->GetMaterials()[i].Albedo;
			m_Data.Materials[i].EmittedColor = renderer.m_Scene->GetMaterials()[i].EmittedColor;
			m_Data.Materials[i].Roughness = renderer.m_Scene->GetMaterials()[i].Roughness;
			m_Data.Materials[i].Shine = renderer.m_Scene->GetMaterials()[i].Shine;
			m_Data.Materials[i].RefractiveIndex = renderer.m_Scene->GetMaterials()[i].RefractiveIndex;
		}

		m_Data.SphereCount = static_cast<uint32_t>(renderer.m_Scene->GetSpheres().size());

		for (uint32_t i = 0; i < renderer.m_Scene->GetSpheres().size(); i++)
		{
			m_Data.Spheres[i].Position = renderer.m_Scene->GetSpheres()[i].Position;
			m_Data.Spheres[i].Radius = renderer.m_Scene->GetSpheres()[i].Radius;
			m_Data.Spheres[i].MaterialIndex = renderer.m_Scene->GetSpheres()[i].MaterialIndex;
		}

		const std::vector<Mesh>& sceneMeshes = renderer.m_Scene->GetMeshes();

		m_Data.MeshCount = static_cast<uint32_t>(sceneMeshes.size());

		for (uint32_t i = 0; i < sceneMeshes.size(); i++)
		{
			m_Data.Meshes[i].TriangleCount = sceneMeshes[i].Triangles.size();

			for (uint32_t j = 0; j < sceneMeshes[i].Triangles.size(); j++)
			{
				m_Data.Meshes[i].Triangles[j].V0 = sceneMeshes[i].Triangles[j].V0;
				m_Data.Meshes[i].Triangles[j].V1 = sceneMeshes[i].Triangles[j].V1;
				m_Data.Meshes[i].Triangles[j].V2 = sceneMeshes[i].Triangles[j].V2;
				m_Data.Meshes[i].Triangles[j].MaterialIndex = sceneMeshes[i].Triangles[j].MaterialIndex;
			}
		}
	}

	ShaderData ShaderDataManager::GetData() const
	{
		return m_Data;
	}
}