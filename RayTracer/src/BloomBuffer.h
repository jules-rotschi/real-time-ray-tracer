#pragma once

#include <vector>

#include "renderer/Texture.h"

namespace RayTracer
{
	class BloomBuffer
	{
	public:
		~BloomBuffer();

		void Init(uint32_t width, uint32_t height, uint32_t mipChainLength);
		void ShutDown();

		void Resize(uint32_t width, uint32_t height);

		const std::vector<GUI::Renderer::Texture>& GetMipChain() const;

	private:
		std::vector<GUI::Renderer::Texture> m_MipChain;
	};
}

