#pragma once

#include <random>
#include <stdint.h>

#include "Vector3.h"

namespace RayTracer
{
	class Random
	{
	public:
		static double Double(uint32_t& seed);
		static double Double(uint32_t& seed, double min, double max);

		static Vector3 Vector(uint32_t& seed);
		static Vector3 Vector(uint32_t& seed, double min, double max);

		static Vector3 VectorInUnitDisk(uint32_t& seed);
		static Vector3 VectorInUnitSphere(uint32_t& seed);

	private:
		static uint32_t PCG_Hash(uint32_t input);
	};
}
