#pragma once

#include <stdint.h>

#include <glad/gl.h>

namespace GUI::Renderer
{
	template <typename T>
	class UniformBuffer
	{
	public:
		UniformBuffer()
		{
			glGenBuffers(1, &m_Handle);
			glBindBuffer(GL_UNIFORM_BUFFER, m_Handle);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(T), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		void BindTo(uint32_t bindingPoint) const
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_Handle);
		}

		void SetData(const T& data) const
		{
			glBindBuffer(GL_UNIFORM_BUFFER, m_Handle);
			glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		GLuint GetHandle() const
		{
			return m_Handle;
		}

	private:
		GLuint m_Handle = 0;
	};
}