#pragma once

#include <stdint.h>
#include <vector>

#include "Vector3.h"

namespace RayTracer
{
	class Camera
	{
	public:
		struct Sensor
		{
			float Width = 0.036f;
			float Height = 0.024f;
			int Sensitivity = 800;
			float Gamma = 0.45f;
		};

		struct Lens
		{
			float FocalLength = 0.05f;
			float FocusDistance = 1.0f;
			float Aperture = 2.8f;
		};

	public:
		Camera() = delete;
		Camera(
			const Vector3& position,
			const Vector3& lensDirection,
			const Vector3& viewUp,
			const Sensor& sensor,
			const Lens& lens,
			float shutterSpeed
		);

		Vector3 GetPosition() const;
		Trihedron GetTrihedron() const;

		const Sensor& GetSensor() const;
		const Lens& GetLens() const;

		Sensor& GetSensor();
		Lens& GetLens();

		float GetNeutralDensity() const;

		void AddNDFilter(float density);
		void RemoveNDFilter(float density);

		void ResetSettings();

		void MoveForward(double dt);
		void MoveBackward(double dt);
		void MoveLeft(double dt);
		void MoveRight(double dt);

		void Rotate(float panRotation, float tiltRotation, double dt);

		void OnUpdate(bool& hasMoved);
		void OnResize(uint32_t width, uint32_t height);

	private:
		Vector3 m_Position;
		Trihedron m_Trihedron;

		Sensor m_Sensor;
		Lens m_Lens;

		uint32_t m_ImageWidth = 1;
		uint32_t m_ImageHeight = 1;

		std::vector<Vector3> m_VirtualPixelPositions;
		float m_VirtualPixelDimension;

		float m_LastFocalLength = m_Lens.FocalLength;
		float m_LastFocusDistance = m_Lens.FocusDistance;
		float m_LastAperture = m_Lens.Aperture;

		float m_ShutterSpeed = 1.0f / 50.0f;
		float m_NeutralDensityValue = 0;

		float m_Speed = 10.0;
		float m_RotationSpeed = 10.0;

		friend class ShaderDataManager;
	};
}

