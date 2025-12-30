#include "Window.h"

#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace GUI
{
	Window::Window(const WindowSpecification& specification)
		: m_Specification(specification) {}

	void Window::Create()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

		m_Handle = glfwCreateWindow(m_Specification.Width, m_Specification.Height, m_Specification.Title.c_str(), nullptr, nullptr);
		if (!m_Handle)
		{
			std::cerr << "Failed to create GLFW window.\n";
			return;
		}

		glfwMakeContextCurrent(m_Handle);
		glfwSwapInterval(1);
	}

	void Window::Destroy() const
	{
		ImGui::DestroyContext();
	}

	void Window::Update() const
	{
		glfwSwapBuffers(m_Handle);
	}

	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(m_Handle);
	}

	GLFWwindow* Window::GetHandle() const
	{
		return m_Handle;
	}
}
