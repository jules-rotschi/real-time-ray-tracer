#pragma once

#include <string>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

namespace GUI
{
	struct WindowSpecification
	{
		std::string Title;
		uint32_t Width = 1280;
		uint32_t Height = 720;
	};

	class Window
	{
	public:
		Window(const WindowSpecification& specification = WindowSpecification());

		void Create();
		void Destroy() const;

		void Update() const;

		bool ShouldClose() const;

		GLFWwindow* GetHandle() const;

	private:
		WindowSpecification m_Specification;
		GLFWwindow* m_Handle = nullptr;
	};
}