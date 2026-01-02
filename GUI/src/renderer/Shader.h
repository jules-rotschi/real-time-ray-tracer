#pragma once

#include <filesystem>

#include <glad/gl.h>

namespace GUI::Renderer
{
	class Shader
	{
	public:
		Shader(const std::filesystem::path& path);

		void Use() const;
		void Compute(GLuint numGroupsX, GLuint numGroupsY, GLuint numGroupsZ, GLbitfield barriers) const;

	private:
		GLuint m_ShaderHandle = 0;
		GLuint m_ProgramHandle = 0;
	};
}