#pragma once

namespace RayTracer
{
	struct Vector3
	{
		union
		{
			double X = 0.0;
			double R;
		};
		union
		{
			double Y = 0.0;
			double G;
		};
		union
		{
			double Z = 0.0;
			double B;
		};

		Vector3() = default;
		Vector3(double x, double y, double z);

		bool IsNearlyNull() const;

		double GetLength() const;
		double GetSquaredLength() const;

		Vector3 GetUnitVector() const;

		Vector3 Reflect(const Vector3& normal) const;
		Vector3 Refract(const Vector3& unitNormal, double refractiveIndicesRatio) const;

		Vector3 Rotate(double angle, const Vector3& axis) const;

		Vector3& operator+=(const Vector3& v);
		Vector3& operator-=(const Vector3& v);
		Vector3& operator*=(double t);
		Vector3& operator/=(double t);

		friend Vector3 operator+(Vector3 v1, const Vector3& v2);
		friend Vector3 operator-(Vector3 v1, const Vector3& v2);
		friend Vector3 operator*(Vector3 v, double t);
		friend Vector3 operator*(double t, Vector3 v);
		friend Vector3 operator/(Vector3 v, double t);

		friend Vector3 operator-(const Vector3& v);

		friend double Dot(const Vector3& v1, const Vector3& v2);
		friend Vector3 Cross(const Vector3& v1, const Vector3& v2);
	};

	struct Trihedron
	{
		Vector3 E1;
		Vector3 E2;
		Vector3 E3;
	};
}