#include "Input.h"

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Application.h"

namespace GUI::Input
{
	bool IsKeyDown(char key)
	{
		GLFWwindow* windowHandle = Application::Get().GetWindow().GetHandle();
		int state = glfwGetKey(windowHandle, key);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool IsMouseButtonPressed(int button)
	{
		GLFWwindow* windowHandle = Application::Get().GetWindow().GetHandle();
		int state = glfwGetMouseButton(windowHandle, button);
		return state == GLFW_PRESS;
	}

	ScreenPosition GetMousePosition()
	{
		GLFWwindow* windowHandle = Application::Get().GetWindow().GetHandle();

		double x;
		double y;
		glfwGetCursorPos(windowHandle, &x, &y);
		return { x, y };
	}
}