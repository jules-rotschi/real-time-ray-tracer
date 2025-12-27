#include "Object.h"

namespace RayTracer
{
	Object::Object(uint32_t materialIndex)
		: m_MaterialIndex(materialIndex) {}

	uint32_t Object::GetMaterialIndex() const
	{
		return m_MaterialIndex;
	}

	uint32_t& Object::r_GetMaterialIndex()
	{
		return m_MaterialIndex;
	}
}