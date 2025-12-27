#include "Vector3.h"

#include <cmath>

namespace RayTracer
{
	Vector3::Vector3(double x, double y, double z)
		: X(x), Y(y), Z(z) { }

	bool Vector3::IsNearlyNull() const
	{
		bool isXNearlyNull = X * X < 1e-3;
		bool isYNearlyNull = Y * Y < 1e-3;
		bool isZNearlyNull = Z * Z < 1e-3;
		return isXNearlyNull && isYNearlyNull && isZNearlyNull;
	}

	double Vector3::GetLength() const
	{
		return std::sqrt(GetSquaredLength());
	}

	double Vector3::GetSquaredLength() const
	{
		return X * X + Y * Y + Z * Z;
	}

	Vector3 Vector3::GetUnitVector() const
	{
		return *this / GetLength();
	}

	Vector3 Vector3::Reflect(const Vector3& normal) const
	{
		return *this - 2 * Dot(*this, normal) * normal;
	}

	Vector3 Vector3::Refract(const Vector3& unitNormal, double refractiveIndicesRatio) const
	{
		Vector3 orthogonalComponent = refractiveIndicesRatio * (*this + Dot(*this, -unitNormal) * unitNormal);
		Vector3 collinearComponent = std::sqrt(1 - orthogonalComponent.GetSquaredLength()) * -unitNormal;
		return orthogonalComponent + collinearComponent;
	}

	Vector3 Vector3::Rotate(double angle, const Vector3& axis) const
	{
		Vector3 u = axis.GetUnitVector();
		double c = std::cos(angle);
		double s = std::sin(angle);

		return {
			X * (u.X * u.X * (1 - c) + c)
			+ Y * (u.X * u.Y * (1 - c) - u.Z * s)
			+ Z * (u.X * u.Z * (1 - c) + u.Y * s),
			X * (u.X * u.Y * (1 - c) + u.Z * s)
			+ Y * (u.Y * u.Y * (1 - c) + c)
			+ Z * (u.Y * u.Z * (1 - c) - u.X * s),
			X * (u.X * u.Z * (1 - c) - u.Y * s)
			+ Y * (u.Y * u.Z * (1 - c) + u.X * s)
			+ Z * (u.Z * u.Z * (1 - c) + c)
		};
	}

	Vector3& Vector3::operator+=(const Vector3& v)
	{
		X += v.X;
		Y += v.Y;
		Z += v.Z;
		return *this;
	}

	Vector3& Vector3::operator-=(const Vector3& v)
	{
		*this += -v;
		return *this;
	}

	Vector3& Vector3::operator*=(double t)
	{
		X *= t;
		Y *= t;
		Z *= t;
		return *this;
	}

	Vector3& Vector3::operator/=(double t)
	{
		*this *= 1 / t;
		return *this;
	}

	Vector3 operator+(Vector3 v1, const Vector3& v2)
	{
		v1 += v2;
		return v1;
	}

	Vector3 operator-(Vector3 v1, const Vector3& v2)
	{
		v1 -= v2;
		return v1;
	}

	Vector3 operator*(Vector3 v, double t)
	{
		v *= t;
		return v;
	}

	Vector3 operator*(double t, Vector3 v)
	{
		return v * t;
	}

	Vector3 operator/(Vector3 v, double t)
	{
		v /= t;
		return v;
	}

	Vector3 operator-(const Vector3& v)
	{
		return -1 * v;
	}

	double Dot(const Vector3& v1, const Vector3& v2)
	{
		return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
	}

	Vector3 Cross(const Vector3& v1, const Vector3& v2)
	{
		return {
			v1.Y * v2.Z - v1.Z * v2.Y,
			v1.Z * v2.X - v1.X * v2.Z,
			v1.X * v2.Y - v1.Y * v2.X
		};
	}
}