#include "Random.h"

#include <limits>

namespace RayTracer
{
	double Random::Double(uint32_t& seed)
	{
		seed = PCG_Hash(seed);
		return static_cast<double>(seed) / static_cast<double>(std::numeric_limits<uint32_t>::max());
	}

	double Random::Double(uint32_t& seed, double min, double max)
	{
		return min + (max - min) * Double(seed);
	}

	Vector3 Random::Vector(uint32_t& seed)
	{
		return { Double(seed), Double(seed), Double(seed) };
	}

	Vector3 Random::Vector(uint32_t& seed, double min, double max)
	{
		return { Double(seed, min, max), Double(seed, min, max), Double(seed, min, max) };
	}

	Vector3 Random::VectorInUnitDisk(uint32_t& seed)
	{
		while (true) {
			Vector3 randomVector = { Double(seed, -1.0, 1.0), Double(seed, -1.0, 1.0), 0.0 };
			if (randomVector.GetSquaredLength() < 1)
				return randomVector;
		}
	}

	Vector3 Random::VectorInUnitSphere(uint32_t& seed)
	{
		while (true) {
			Vector3 randomVector = Vector(seed, -1, 1);
			if (randomVector.GetSquaredLength() < 1 && randomVector.GetSquaredLength() > 1e-160) {
				return randomVector.GetUnitVector();
			}
		}
	}

	uint32_t Random::PCG_Hash(uint32_t input)
	{
		uint32_t state = input * 747796405u + 2891336453u;
		uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
		return (word >> 22u) ^ word;
	}
}
