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
			double Width = 0.036;
			double Height = 0.024;
			int Sensivity = 800;
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
			double shutterSpeed
		);

		double GetExposure() const;
		Vector3 GetPixel(const Vector3& luminance) const;

		Vector3 GetRayOrigin(bool depthOfField, uint32_t& seed) const;
		Vector3 GetVirtualPixelPosition(int x, int y, bool antialiasing, uint32_t& seed) const;
		Vector3 GetVirtualPixelPosition(int x, int y) const;

		Vector3 GetPosition() const;
		const Sensor& GetSensor() const;
		const Lens& GetLens() const;
		Sensor& GetSensor();
		Lens& GetLens();

		double GetNeutralDensity() const;

		void AddNDFilter(double density);
		void RemoveNDFilter(double density);

		void ResetSettings();

		void MoveForward(double dt);
		void MoveBackward(double dt);
		void MoveLeft(double dt);
		void MoveRight(double dt);

		void Rotate(double panRotation, double tiltRotation, double dt);

		void OnUpdate(bool& hasMoved);
		void OnResize(uint32_t width, uint32_t height);

	private:
		void ComputeVirtualPixelPositions();

	private:
		Vector3 m_Position;
		Trihedron m_Trihedron;

		Sensor m_Sensor;
		Lens m_Lens;

		uint32_t m_ImageWidth = 1;
		uint32_t m_ImageHeight = 1;

		std::vector<Vector3> m_VirtualPixelPositions;
		double m_VirtualPixelDimension;

		double m_LastFocalLength = m_Lens.FocalLength;
		double m_LastFocusDistance = m_Lens.FocusDistance;
		double m_LastAperture = m_Lens.Aperture;

		double m_ShutterSpeed = 1.0 / 50.0;
		double m_NeutralDensityValue = 0;

		double m_Speed = 10.0;
		double m_RotationSpeed = 10.0;
	};
}

