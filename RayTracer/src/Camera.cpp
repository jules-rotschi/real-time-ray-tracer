#include "Camera.h"

#include <cmath>
#include <numbers>

namespace RayTracer
{
	Camera::Camera(
		const Vector3& position,
		const Vector3& lensDirection,
		const Vector3& viewUp,
		const Sensor& sensor,
		const Lens& lens,
		float shutterSpeed
	) : m_Position(position), m_Sensor(sensor), m_Lens(lens), m_ShutterSpeed(shutterSpeed)
	{
		m_Trihedron.E3 = -lensDirection.GetUnitVector();
		m_Trihedron.E1 = Cross(viewUp, m_Trihedron.E3).GetUnitVector();
		m_Trihedron.E2 = Cross(m_Trihedron.E3, m_Trihedron.E1);
	}

	Vector3 Camera::GetPosition() const
	{
		return m_Position;
	}

	Trihedron Camera::GetTrihedron() const
	{
		return m_Trihedron;
	}

	const Camera::Sensor& Camera::GetSensor() const
	{
		return m_Sensor;
	}

	const Camera::Lens& Camera::GetLens() const
	{
		return m_Lens;
	}

	Camera::Sensor& Camera::GetSensor()
	{
		return m_Sensor;
	}

	Camera::Lens& Camera::GetLens()
	{
		return m_Lens;
	}

	float Camera::GetNeutralDensity() const
	{
		return m_NeutralDensityValue;
	}

	void Camera::AddNDFilter(float density)
	{
		m_NeutralDensityValue += density;
	}

	void Camera::RemoveNDFilter(float density)
	{
		if (density >= m_NeutralDensityValue)
		{
			m_NeutralDensityValue = 0.0f;
			return;
		}

		m_NeutralDensityValue -= density;
	}

	void Camera::ResetSettings()
	{
		m_Sensor = Sensor{};
		m_Lens = Lens{};
		m_NeutralDensityValue = 0;
	}

	void Camera::MoveForward(double dt)
	{
		m_Position += -m_Trihedron.E3 * m_Speed * static_cast<float>(dt);
	}

	void Camera::MoveBackward(double dt)
	{
		m_Position += m_Trihedron.E3 * m_Speed * static_cast<float>(dt);
	}

	void Camera::MoveLeft(double dt)
	{
		m_Position += -m_Trihedron.E1 * m_Speed * static_cast<float>(dt);
	}

	void Camera::MoveRight(double dt)
	{
		m_Position += m_Trihedron.E1 * m_Speed * static_cast<float>(dt);
	}

	void Camera::Rotate(float panRotation, float tiltRotation, double dt)
	{
		Vector3 currentUnitLensDirection = -m_Trihedron.E3;

		Vector3 newUnitLensDirection = currentUnitLensDirection
			.Rotate(panRotation * m_RotationSpeed * static_cast<float>(dt), m_Trihedron.E2)
			.Rotate(tiltRotation * m_RotationSpeed * static_cast<float>(dt), m_Trihedron.E1);

		m_Trihedron.E3 = -newUnitLensDirection;
		m_Trihedron.E1 = Cross({ 0.0, 1.0, 0.0 }, m_Trihedron.E3);
		m_Trihedron.E2 = Cross(m_Trihedron.E3, m_Trihedron.E1);
	}

	void Camera::OnUpdate(bool& hasMoved)
	{
		if (m_LastFocalLength != m_Lens.FocalLength
			|| m_LastFocusDistance != m_Lens.FocusDistance
			|| m_LastAperture != m_Lens.Aperture
		)
		{
			m_LastFocalLength = m_Lens.FocalLength;
			m_LastFocusDistance = m_Lens.FocusDistance;
			m_LastAperture = m_Lens.Aperture;
			hasMoved = true;
		}

		if (hasMoved)
			m_InvalidPixelPositions = true;
	}

	void Camera::OnResize(uint32_t width, uint32_t height)
	{
		if (m_ImageWidth == width && m_ImageHeight == height)
			return;

		m_ImageWidth = width;
		m_ImageHeight = height;

		m_InvalidPixelPositions = true;
	}

	void Camera::SetVirtualPixelPositionsCached()
	{
		m_InvalidPixelPositions = false;
	}
}