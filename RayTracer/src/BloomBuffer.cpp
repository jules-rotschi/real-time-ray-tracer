#include "BloomBuffer.h"

namespace RayTracer
{
	BloomBuffer::~BloomBuffer()
	{
		ShutDown();
	}

	void BloomBuffer::Init(uint32_t width, uint32_t height, uint32_t mipChainLength)
	{
		for (uint32_t i = 0; i < mipChainLength; i++)
		{
			m_MipChain.emplace_back(width, height, GUI::Renderer::Texture::FilteringMode::LINEAR);

			width /= 2;
			height /= 2;
		}
	}

	void BloomBuffer::ShutDown()
	{
		for (GUI::Renderer::Texture& texture : m_MipChain)
		{
			texture.ShutDown();
		}
		m_MipChain.clear();
	}

	void BloomBuffer::Resize(uint32_t width, uint32_t height)
	{
		uint32_t mipChainLength = static_cast<uint32_t>(m_MipChain.size());
		ShutDown();
		Init(width, height, mipChainLength);
	}

	const std::vector<GUI::Renderer::Texture>& BloomBuffer::GetMipChain() const
	{
		return m_MipChain;
	}
}