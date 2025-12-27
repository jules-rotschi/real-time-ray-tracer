#include "Camera.h"

#include <cmath>
#include <numbers>

#include "Random.h"

namespace RayTracer
{
	Camera::Camera(
		const Vector3& position,
		const Vector3& lensDirection,
		const Vector3& viewUp,
		const Sensor& sensor,
		const Lens& lens,
		double shutterSpeed
	) : m_Position(position), m_Sensor(sensor), m_Lens(lens), m_ShutterSpeed(shutterSpeed)
	{
		m_Trihedron.E3 = -lensDirection.GetUnitVector();
		m_Trihedron.E1 = Cross(viewUp, m_Trihedron.E3).GetUnitVector();
		m_Trihedron.E2 = Cross(m_Trihedron.E3, m_Trihedron.E1);

		ComputeVirtualPixelPositions();
	}

	double Camera::GetExposure() const
	{
		return m_Sensor.Sensivity * m_ShutterSpeed / (270 * m_Lens.Aperture * m_Lens.Aperture * std::pow(10, m_NeutralDensityValue));
	}

	Vector3 Camera::GetPixel(const Vector3& luminance) const
	{
		Vector3 linear_value = luminance * std::numbers::pi * GetExposure() / 0.89;
		linear_value.R = linear_value.R > 1 ? 1 : linear_value.R;
		linear_value.G = linear_value.G > 1 ? 1 : linear_value.G;
		linear_value.B = linear_value.B > 1 ? 1 : linear_value.B;

		Vector3 gamma_corrected_value;
		gamma_corrected_value.R = std::pow(linear_value.R, m_Sensor.Gamma);
		gamma_corrected_value.G = std::pow(linear_value.G, m_Sensor.Gamma);
		gamma_corrected_value.B = std::pow(linear_value.B, m_Sensor.Gamma);

		return gamma_corrected_value;
	}

	Vector3 Camera::GetRayOrigin(bool depthOfField, uint32_t& seed) const
	{
		if (!depthOfField)
			return m_Position;

		double apertureRadius = (m_Lens.FocalLength / m_Lens.Aperture) / 2;
		return m_Position + Random::VectorInUnitDisk(seed) * apertureRadius;
	}

	Vector3 Camera::GetVirtualPixelPosition(int x, int y, bool antialiasing, uint32_t& seed) const
	{
		Vector3 pixelCenter = m_VirtualPixelPositions[x + y * m_ImageWidth];

		if (!antialiasing)
			return pixelCenter;

		Vector3 offset(0.5 * Random::Double(seed), 0.5 * Random::Double(seed), 0);
		return pixelCenter + offset * m_VirtualPixelDimension;
	}

	Vector3 Camera::GetVirtualPixelPosition(int x, int y) const
	{
		uint32_t fakeSeed = 1;
		return GetVirtualPixelPosition(x, y, false, fakeSeed);
	}

	Vector3 Camera::GetPosition() const
	{
		return m_Position;
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

	double Camera::GetNeutralDensity() const
	{
		return m_NeutralDensityValue;
	}

	void Camera::AddNDFilter(double density)
	{
		m_NeutralDensityValue += density;
	}

	void Camera::RemoveNDFilter(double density)
	{
		if (density >= m_NeutralDensityValue)
			return;

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
		m_Position += -m_Trihedron.E3 * m_Speed * dt;
	}

	void Camera::MoveBackward(double dt)
	{
		m_Position += m_Trihedron.E3 * m_Speed * dt;
	}

	void Camera::MoveLeft(double dt)
	{
		m_Position += -m_Trihedron.E1 * m_Speed * dt;
	}

	void Camera::MoveRight(double dt)
	{
		m_Position += m_Trihedron.E1 * m_Speed * dt;
	}

	void Camera::Rotate(double panRotation, double tiltRotation, double dt)
	{
		Vector3 currentUnitLensDirection = -m_Trihedron.E3;

		Vector3 newUnitLensDirection = currentUnitLensDirection
			.Rotate(panRotation * m_RotationSpeed * dt, m_Trihedron.E2)
			.Rotate(tiltRotation * m_RotationSpeed * dt, m_Trihedron.E1);

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
			ComputeVirtualPixelPositions();
	}

	void Camera::OnResize(uint32_t width, uint32_t height)
	{
		if (m_ImageWidth == width && m_ImageHeight == height)
			return;

		m_ImageWidth = width;
		m_ImageHeight = height;

		ComputeVirtualPixelPositions();
	}

	void Camera::ComputeVirtualPixelPositions()
	{
		m_VirtualPixelPositions.resize(m_ImageWidth * m_ImageHeight);

		double sensorRatio = m_Sensor.Width / m_Sensor.Height;
		double imageRatio = static_cast<double>(m_ImageWidth) / static_cast<double>(m_ImageHeight);

		double sensorActiveWidth = m_Sensor.Width;
		double sensorActiveHeight = m_Sensor.Height;

		if (sensorRatio < imageRatio)
			sensorActiveHeight = m_Sensor.Width / imageRatio;

		else if (sensorRatio > imageRatio)
			sensorActiveWidth = m_Sensor.Height * imageRatio;

		double viewportDimensionRatio = m_Lens.FocusDistance / m_Lens.FocalLength;
		double viewportWidth = viewportDimensionRatio * sensorActiveWidth;
		double viewportHeight = viewportDimensionRatio * sensorActiveHeight;

		m_VirtualPixelDimension = viewportWidth / m_ImageWidth;

		for (uint32_t y = 0; y < m_ImageHeight; y++)
		{
			for (uint32_t x = 0; x < m_ImageWidth; x++)
			{
				Vector3 viewportBegin =
					m_Position - (viewportWidth / 2) * m_Trihedron.E1
					+ (viewportHeight / 2) * m_Trihedron.E2
					- m_Lens.FocusDistance * m_Trihedron.E3;

				Vector3 pixelBegin =
					viewportBegin + x * m_VirtualPixelDimension * m_Trihedron.E1 - y * m_VirtualPixelDimension * m_Trihedron.E2;

				Vector3 pixelCenter =
					pixelBegin + (m_VirtualPixelDimension / 2) * (m_Trihedron.E1 - m_Trihedron.E2);

				m_VirtualPixelPositions[x + y * m_ImageWidth] = pixelCenter;
			}
		}
	}
}