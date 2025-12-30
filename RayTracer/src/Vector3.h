#pragma once

namespace RayTracer
{
	struct Vector3
	{
		union
		{
			float X = 0.0;
			float R;
		};
		union
		{
			float Y = 0.0;
			float G;
		};
		union
		{
			float Z = 0.0;
			float B;
		};

		Vector3() = default;
		Vector3(float x, float y, float z);

		bool IsNearlyNull() const;

		float GetLength() const;
		float GetSquaredLength() const;

		Vector3 GetUnitVector() const;

		Vector3 Reflect(const Vector3& normal) const;
		Vector3 Refract(const Vector3& unitNormal, float refractiveIndicesRatio) const;

		Vector3 Rotate(float angle, const Vector3& axis) const;

		Vector3& operator+=(const Vector3& v);
		Vector3& operator-=(const Vector3& v);
		Vector3& operator*=(float t);
		Vector3& operator/=(float t);

		friend Vector3 operator+(Vector3 v1, const Vector3& v2);
		friend Vector3 operator-(Vector3 v1, const Vector3& v2);
		friend Vector3 operator*(Vector3 v, float t);
		friend Vector3 operator*(float t, Vector3 v);
		friend Vector3 operator/(Vector3 v, float t);

		friend Vector3 operator-(const Vector3& v);

		friend float Dot(const Vector3& v1, const Vector3& v2);
		friend Vector3 Cross(const Vector3& v1, const Vector3& v2);
	};

	struct Trihedron
	{
		Vector3 E1;
		Vector3 E2;
		Vector3 E3;
	};
}