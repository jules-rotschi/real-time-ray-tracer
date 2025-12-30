#pragma once

#include <stdint.h>
#include <vector>

#include <glad/gl.h>

#include "Vector3.h"

namespace RayTracer
{
	class Renderer;
	class Camera;
	class Scene;

	struct Vec4
	{
		float X = 0.0f;
		float Y = 0.0f;
		float Z = 0.0f;
		float W = 0.0f;

		Vec4& operator=(const Vector3& v)
		{
			X = v.X;
			Y = v.Y;
			Z = v.Z;
			W = 0.0f;
			return *this;
		}
	};
	static_assert(sizeof(Vec4) == 16);

	struct RendererSettings
	{
		uint32_t Preview = false;
		uint32_t PreviewMode = 1;
		uint32_t Accumulation = true;
		uint32_t Antialiasing = true;
		uint32_t DepthOfField = true;
		uint32_t FocusPeaking = false;
		uint32_t Depth = 5;

		uint32_t _padding = 0;
	};
	static_assert(sizeof(RendererSettings) == 2 * 16);

	struct CameraData
	{
		Vec4 Position;
		Vec4 E1;
		Vec4 E2;
		Vec4 E3;
		float SensorWidth = 0.036f;
		float SensorHeight = 0.024f;
		uint32_t Sensitivity = 800;
		float Gamma = 0.45f;
		float FocalLength = 0.05f;
		float FocusDistance = 1.0f;
		float Aperture = 2.8f;
		float ShutterSpeed = 0.02f;
		float NeutralDensityValue = 0.0f;

		uint32_t _padding[3];
	};
	static_assert(sizeof(CameraData) == 7 * 16);

	struct MaterialData
	{
		uint32_t IsOpaque = 1;

		uint32_t _padding1[3];

		Vec4 Albedo;
		Vec4 EmittedColor;
		float Roughness = 0.0f;
		float Shine = 0.0f;
		float RefractiveIndex = 1.0f;

		uint32_t _padding2 = 0;
	};
	static_assert(sizeof(MaterialData) == 4 * 16);

	struct SphereData
	{
		Vec4 Position;
		float Radius = 1.0f;
		uint32_t MaterialIndex = 0;

		uint32_t _padding[2];
	};
	static_assert(sizeof(SphereData) == 2 * 16);

	struct ShaderData
	{
		RendererSettings Settings;    // 0 + 32 = 32
		uint32_t FrameIndex = 1;      // 32 + 4 = 36

		uint32_t _padding1[3];	      // 36 + 12 = 48

		CameraData Camera;		      // 48 + 112 = 160

		Vec4 SkyColor;				  // 160 + 16 = 176
		
		MaterialData Materials[10];	  // 176 + 10 * 64 = 816
		uint32_t MaterialsCount = 0;  // 816 + 4 = 820

		uint32_t _padding2[3];		  // 820 + 12 = 832

		SphereData Spheres[10];		  // 832 + 10 * 32 = 1152
		uint32_t SpheresCount = 0;	  // 1152 + 4 = 1156

		uint32_t _padding3[3];		  // 1156 + 12 = 1168
	};
	static_assert(sizeof(ShaderData) == 73 * 16);

	struct LuminanceData
	{
		float R = 0.0f;
		float G = 0.0f;
		float B = 0.0f;
		float _padding = 0.0f;
	};

	class ShaderDataManager
	{
	public:
		void UpdateData(const Renderer& renderer);

		ShaderData GetData() const;

	private:
		ShaderData m_Data;
	};
}
