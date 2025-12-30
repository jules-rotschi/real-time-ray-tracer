#pragma once

#include <stdint.h>

#include <glad/gl.h>

namespace GUI::Renderer
{
	template <typename T>
	class ShaderStorageBuffer
	{
	public:
		ShaderStorageBuffer()
		{
			glGenBuffers(1, &m_Handle);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(T), nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		void BindTo(uint32_t bindingPoint) const
		{
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_Handle);
		}

		void Resize(size_t bytes) const
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_Handle);
			glBufferData(GL_SHADER_STORAGE_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		}

		GLuint GetHandle() const
		{
			return m_Handle;
		}

	private:
		GLuint m_Handle = 0;
	};
}