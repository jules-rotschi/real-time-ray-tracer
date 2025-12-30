#include "AppLayer.h"

#include <string>
#include <map>
#include <chrono>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Application.h"
#include "renderer/Image.h"

#include "Vector3.h"

#include "Encoder.h"

AppLayer::AppLayer()
    : m_Scene({ 10.0f, 20.0f, 40.0f }),
    m_Camera(
        { 0.0f, 2.0f, 8.0f },
        { 0.0f, -1.0f, -5.0f },
        { 0.0f, 1.0f, 0.0f },
        RayTracer::Camera::Sensor{},
        { 0.05f, 8.0f, 2.8f },
        1.0f / 50.0f
    )
{
    std::map<std::string_view, uint32_t> materials;

    // Diffuse colors
    uint32_t grey = m_Scene.AddMaterial({ true, { 0.5f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f, 1.0f });
    uint32_t red = m_Scene.AddMaterial({ true, { 0.8f, 0.4f, 0.4f }, { 0.0f, 0.0f, 0.0f }, 0.0f, 0.0f, 1.0f });

    // Metals
    uint32_t metal = m_Scene.AddMaterial({ true, { 0.9f, 0.9f, 0.9f }, { 0.0f, 0.0f, 0.0f }, 0.01f, 1.0f, 1.0f });
    uint32_t fuzzyMetal = m_Scene.AddMaterial({ true, { 0.9f, 0.9f, 0.9f }, { 0.0f, 0.0f, 0.0f }, 0.1f, 1.0f, 1.0f });

    // Clears
    uint32_t glass = m_Scene.AddMaterial({ false, { 0.8f, 1.0f, 0.8f }, { 0.0f, 0.0f, 0.0f }, 0.1f, 1.0f, 1.52f });

    // Lights
    uint32_t whiteLight = m_Scene.AddMaterial({ true, { 0.1f, 0.1f, 0.1f }, { 20000.0f, 20000.0f, 20000.0f }, 1.0f, 0.0f, 1.0f });
    uint32_t warmLight = m_Scene.AddMaterial({ true, { 0.1f, 0.1f, 0.1f }, { 20000.0f, 10000.0f, 5000.0f }, 1.0f, 0.0f, 1.0f });

    // Shiny
    uint32_t ceramic = m_Scene.AddMaterial({ true, { 0.9f, 0.9f, 0.9f }, { 0.0f, 0.0f, 0.0f }, 0.01f, 0.5f, 1.0f });
    uint32_t tiling = m_Scene.AddMaterial({ true, { 0.4f, 0.5f, 0.5f }, { 0.0f, 0.0f, 0.0f }, 0.3f, 0.2f, 1.0f });

    m_Scene.AddSphere({ { 0.0f, -100.5f, 0.0f }, 100.0f, tiling }); // ground
    m_Scene.AddSphere({ { -1.0f, 0.0f, -2.0f }, 0.5f, red });
    m_Scene.AddSphere({ { -1.5f, 0.0f, 0.0f }, 0.5f, ceramic });
    m_Scene.AddSphere({ { 1.5f, 0.0f, -0.5f }, 0.5f, fuzzyMetal });
    m_Scene.AddSphere({ { 0.0f, 0.0f, 0.0f }, 0.5f, glass });
    m_Scene.AddSphere({ { -3.0f, 2.0f, 2.0f }, 1.0f, whiteLight }); // key light
    m_Scene.AddSphere({ { 3.0f, 150.0f, -150.0f }, 50.0f, warmLight }); // back light
}

void AppLayer::OnUpdate(double dt)
{
    bool hasCameraMoved = false;

    GUI::ScreenPosition currentMousePosition = GUI::Input::GetMousePosition();

    if (GUI::Input::IsMouseButtonPressed(2) && currentMousePosition != m_LastMousePosition)
    {
        glfwSetInputMode(GUI::Application::Get().GetWindow().GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL + 2);

        float deltaX = static_cast<float>(currentMousePosition.X - m_LastMousePosition.X) * 0.002f;
        float deltaY = static_cast<float>(currentMousePosition.Y - m_LastMousePosition.Y) * 0.002f;

        m_Camera.Rotate(
            -deltaX,
            -deltaY,
            dt
        );

        hasCameraMoved = true;
    }
    else if (!GUI::Input::IsMouseButtonPressed(2))
    {
        glfwSetInputMode(GUI::Application::Get().GetWindow().GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    m_LastMousePosition = currentMousePosition;

    if (GUI::Input::IsKeyDown('W'))
    {
        m_Camera.MoveForward(dt);
        hasCameraMoved = true;
    }

    if (GUI::Input::IsKeyDown('S'))
    {
        m_Camera.MoveBackward(dt);
        hasCameraMoved = true;
    }

    if (GUI::Input::IsKeyDown('A'))
    {
        m_Camera.MoveLeft(dt);
        hasCameraMoved = true;
    }

    if (GUI::Input::IsKeyDown('D'))
    {
        m_Camera.MoveRight(dt);
        hasCameraMoved = true;
    }

    m_Camera.OnUpdate(hasCameraMoved);
    m_Renderer.OnUpdate(dt);

    if (hasCameraMoved)
    {
        m_Renderer.ResetFrameIndex();
        m_HasUserExported = false;
        m_HasExportSucceeded = false;
    }
}

void AppLayer::OnRender()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if (m_Render)
        Render();

    ImGui::Begin("Renderer settings");

    ImGui::Text("Image dimensions: %d*%dpx", m_ViewportWidth, m_ViewportHeight);
    ImGui::Text("Render duration: %.3fms", m_LastRenderTime);
    ImGui::Text("Accumulation duration: %.1fs", m_Renderer.GetAccumulationTime());

    ImGui::Checkbox("Render", &m_Render);

    ImGui::Checkbox("Preview", &m_Renderer.GetSettings().Preview);

    int previewMode = static_cast<int>(m_Renderer.GetSettings().PreviewMode);

    if (m_Renderer.GetSettings().Preview)
    {
        ImGui::DragInt("Preview mode", &previewMode, 1.0f, 1, 2);
        m_Renderer.GetSettings().PreviewMode = static_cast<RayTracer::Renderer::PreviewMode>(previewMode);
    }

    if (!m_Renderer.GetSettings().Preview)
    {
        int depth = m_Renderer.GetSettings().Depth;

        ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulation);
        ImGui::DragInt("Depth", &depth, 1.0f, 1, 100);
        ImGui::Checkbox("Antialiasing", &m_Renderer.GetSettings().Antialiasing);
        ImGui::Checkbox("Depth of field", &m_Renderer.GetSettings().DepthOfField);

        m_Renderer.GetSettings().Depth = depth;
    }

    ImGui::Checkbox("Focus peaking", &m_Renderer.GetSettings().FocusPeaking);
    
    //ImGui::InputText("Filename", &m_ExportFilename[0], 255);

    //if (ImGui::Button("Export PPM"))
    //{
    //    m_HasUserExported = true;
    //    m_HasExportSucceeded = EncodePPM(&m_ExportFilename[0], m_ViewportWidth, m_ViewportHeight, m_Renderer.GetImageData());
    //}

    //if (m_HasUserExported)
    //{
    //    ImGui::Separator();

    //    if (m_HasExportSucceeded)
    //        ImGui::Text("Image has been exported !");
    //    else
    //        ImGui::Text("Failed to export : file already exists.");
    //}

    ImGui::Separator();

    if (ImGui::Button("Reset"))
    {
        m_Renderer.ResetFrameIndex();
        m_HasUserExported = false;
        m_HasExportSucceeded = false;
    }

    ImGui::End();

    ImGui::Begin("Camera settings");

    ImGui::Text("Position : %.2f ; %.2f ; %.2f", m_Camera.GetPosition().X, m_Camera.GetPosition().Y, m_Camera.GetPosition().Z);
    ImGui::Text("Gamma: %.2f", m_Camera.GetSensor().Gamma);
    ImGui::Text("Sensivity: %d ASA", m_Camera.GetSensor().Sensitivity);
    ImGui::Text("Aperture: f/%.1f", m_Camera.GetLens().Aperture);
    ImGui::Text("Focal length: %.0fmm", m_Camera.GetLens().FocalLength * 1000.0f);
    ImGui::Text("Focus distance: %.0fm", m_Camera.GetLens().FocusDistance);
    ImGui::Text("Neutral density: %.1f", m_Camera.GetNeutralDensity());

    ImGui::DragFloat("Gamma", &m_Camera.GetSensor().Gamma, 0.05f, 0.1f, 1.0f);
    ImGui::DragInt("Sensivity", &m_Camera.GetSensor().Sensitivity, 100.0f, 100, 12800);
    ImGui::DragFloat("Aperture", &m_Camera.GetLens().Aperture, 0.1f, 1.0f, 32.0f);
    ImGui::DragFloat("Focal length", &m_Camera.GetLens().FocalLength, 0.005f, 0.014f, 1.0f);
    if (m_Renderer.GetSettings().DepthOfField)
    {
        ImGui::DragFloat("Focus distance", &m_Camera.GetLens().FocusDistance, 0.01f, 0.1f, 100.0f);
    }

    if (ImGui::Button("Add ND"))
    {
        m_Camera.AddNDFilter(0.3f);
    }

    if (ImGui::Button("Remove ND"))
    {
        m_Camera.RemoveNDFilter(0.3f);
    }

    if (ImGui::Button("Reset camera settings"))
    {
        m_Camera.ResetSettings();
    }

    ImGui::End();

    ImGui::Begin("Scene settings");

    RayTracer::Vector3 unitSkyColor = m_Scene.GetSkyColor().GetUnitVector();
    float skyColorIntensity = m_Scene.GetSkyColor().GetLength();

    std::array<float, 3> skyColor = {
        unitSkyColor.R,
        unitSkyColor.G,
        unitSkyColor.B
    };

    ImGui::ColorEdit3("Sky color", &skyColor[0]);
    ImGui::DragFloat("Sky luminosity", &skyColorIntensity, 1.0f, 1.0f, 1000000.0f);

    m_Scene.r_GetSkyColor().R = skyColor[0];
    m_Scene.r_GetSkyColor().G = skyColor[1];
    m_Scene.r_GetSkyColor().B = skyColor[2];
    m_Scene.r_GetSkyColor() *= skyColorIntensity;

    ImGui::Separator();

    ImGui::Text("Materials");

    for (uint32_t i = 0; i < m_Scene.GetMaterials().size(); i++)
    {
        ImGui::PushID(i);

        RayTracer::Material& material = m_Scene.GetMaterials()[i];

        std::array<float, 3> albedo = {
            material.Albedo.R,
            material.Albedo.G,
            material.Albedo.B
        };

        ImGui::BulletText("Material %d", i);
        ImGui::ColorEdit3("Albedo", &albedo[0]);

        material.Albedo.R = albedo[0];
        material.Albedo.G = albedo[1];
        material.Albedo.B = albedo[2];

        if (float emittedColorIntensity = material.EmittedColor.GetLength(); emittedColorIntensity != 0.0f)
        {
            RayTracer::Vector3 unitEmittedColor = material.EmittedColor.GetUnitVector();

            std::array<float, 3> emittedColor = {
                unitEmittedColor.R,
                unitEmittedColor.G,
                unitEmittedColor.B
            };

            ImGui::ColorEdit3("Emitted color", &emittedColor[0]);
            ImGui::DragFloat("Emitted light", &emittedColorIntensity, 1.0f, 1.0f, 1000000.0f);

            material.EmittedColor.R = emittedColor[0];
            material.EmittedColor.G = emittedColor[1];
            material.EmittedColor.B = emittedColor[2];
            material.EmittedColor *= emittedColorIntensity;
        }

        ImGui::DragFloat("Roughness", &material.Roughness, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Shine", &material.Shine, 0.01f, 0.0f, 1.0f);

        if (material.IsOpaque)
        {
            ImGui::DragFloat("Refractive index", &material.RefractiveIndex, 0.1f, 0.1f, 2.0f);
        }

        ImGui::PopID();
    }

    ImGui::Separator();

    ImGui::Text("Objects");

    for (uint32_t i = 0; i < m_Scene.GetSpheres().size(); i++)
    {
        ImGui::PushID(i);

        RayTracer::Sphere& sphere = m_Scene.GetSpheres()[i];

        int sphereMaterial = static_cast<int>(sphere.MaterialIndex);

        ImGui::BulletText("Object %d (sphere)", i);
        ImGui::DragFloat("Position X", &sphere.Position.X, 0.1f);
        ImGui::DragFloat("Position Y", &sphere.Position.Y, 0.1f);
        ImGui::DragFloat("Position Z", &sphere.Position.Z, 0.1f);
        ImGui::DragFloat("Radius", &sphere.Radius, 0.1f, 0.0f, 1000.0f);
        ImGui::DragInt("Material index", &sphereMaterial, 1.0f, 0, static_cast<int>(m_Scene.GetMaterials().size()) - 1);

        sphere.MaterialIndex = sphereMaterial;

        ImGui::PopID();
    }

    if (ImGui::Button("Add sphere"))
    {
        m_Scene.AddSphere({ { 0.0f, 0.0f, 0.0f }, 0.5f, 0 });
    }

    ImGui::End();

    ImGui::Begin("Viewport");

    m_ViewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
    m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

    GUI::Renderer::Image image = m_Renderer.GetImage();

    ImGui::Image(image.GetTexture(), ImVec2(static_cast<float>(image.GetWidth()), static_cast<float>(image.GetHeight())), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f));

    ImGui::End();

    ImGui::Render();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void AppLayer::Render()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();

    m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
    m_Renderer.Render(m_Scene, m_Camera);

    m_LastRenderTime = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - startTime).count()) * 0.001 * 0.001;
}