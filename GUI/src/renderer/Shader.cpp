#include "Shader.h"

#include <fstream>
#include <iostream>

namespace GUI::Renderer
{
	Shader::Shader(const std::filesystem::path& path)
	{
		std::string shaderSource;

		std::ifstream file(path);

		if (!file.is_open())
		{
			std::cerr << "Failed to open file: " << path.string() << std::endl;
			return;
		}

		std::ostringstream contentStream;
		contentStream << file.rdbuf();
	
		shaderSource = contentStream.str();

		m_ShaderHandle = glCreateShader(GL_COMPUTE_SHADER);

		const GLchar* source = shaderSource.c_str();
		glShaderSource(m_ShaderHandle, 1, &source, nullptr);

		glCompileShader(m_ShaderHandle);

		GLint isCompiled = 0;
		glGetShaderiv(m_ShaderHandle, GL_COMPILE_STATUS, &isCompiled);
		if (!isCompiled)
		{
			GLint maxLength = 0;
			glGetShaderiv(m_ShaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(m_ShaderHandle, maxLength, &maxLength, &infoLog[0]);

			std::cerr << infoLog.data() << std::endl;

			glDeleteShader(m_ShaderHandle);

			return;
		}

		m_ProgramHandle = glCreateProgram();
		glAttachShader(m_ProgramHandle, m_ShaderHandle);
		glLinkProgram(m_ProgramHandle);

		GLint isLinked = 0;
		glGetProgramiv(m_ProgramHandle, GL_LINK_STATUS, &isLinked);
		if (!isLinked)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_ProgramHandle, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_ProgramHandle, maxLength, &maxLength, &infoLog[0]);

			std::cerr << infoLog.data() << std::endl;

			glDeleteProgram(m_ProgramHandle);
			glDeleteShader(m_ShaderHandle);

			return;
		}

		glDetachShader(m_ProgramHandle, m_ShaderHandle);
	}

	void Shader::Use() const
	{
		glUseProgram(m_ProgramHandle);
	}
}