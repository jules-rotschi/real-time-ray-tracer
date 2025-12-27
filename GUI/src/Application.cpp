#include "Application.h"

#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace GUI
{
    static Application* s_Application = nullptr;

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        s_Application = this;

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW.\n";
        }

        if (m_Specification.WindowSpecification.Title.empty())
            m_Specification.WindowSpecification.Title = m_Specification.Name;

        m_Window = Window(m_Specification.WindowSpecification);
        m_Window.Create();

        if (!gladLoadGL(glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize GLAD\n";
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_Window.GetHandle(), true);
        ImGui_ImplOpenGL3_Init();
    }

    Application::~Application()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        m_Window.Destroy();

        s_Application = nullptr;
    }

    void Application::Run() const
    {
        float lastTime = GetTime();

        while (!m_Window.ShouldClose())
        {
            glfwPollEvents();

            float currentTime = GetTime();
            float deltaTime = currentTime - lastTime;
            lastTime = currentTime;

            for (const std::unique_ptr<Layer>& layer : m_LayerStack)
            {
                layer->OnUpdate(deltaTime);
            }

            for (const std::unique_ptr<Layer>& layer : m_LayerStack)
            {
                layer->OnRender();
            }

            m_Window.Update();
        }
    }

    const Window& Application::GetWindow() const
    {
        return m_Window;
    }

    Application& Application::Get()
    {
        assert(s_Application);
        return *s_Application;
    }

    float Application::GetTime()
    {
        return static_cast<float>(glfwGetTime());
    }
}
