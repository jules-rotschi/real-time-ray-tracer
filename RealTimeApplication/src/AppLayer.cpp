#include "AppLayer.h"

#include <string>
#include <map>
#include <chrono>
#include <memory>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Application.h"
#include "Image.h"
#include "Input.h"

#include "Vector3.h"
#include "Random.h"
#include "Material.h"
#include "Sphere.h"

#include "Encoder.h"

AppLayer::AppLayer()
    : m_Scene({ 10.0, 20.0, 40.0 }),
    m_Camera(
        { 0.0, 2.0, 8.0 },
        { 0.0, -1.0, -5.0 },
        { 0.0, 1.0, 0.0 },
        RayTracer::Camera::Sensor{},
        { 0.05f, 8.0, 2.8f },
        1.0 / 50.0
    )
{
    std::map<std::string_view, uint32_t> materials;

    // Diffuse colors
    m_Scene.AddMaterial(RayTracer::Opaque({ 0.5, 0.5, 0.5 }, 0.0, 0.0));
    materials["Grey"] = 0;

    m_Scene.AddMaterial(RayTracer::Opaque({ 0.8, 0.4, 0.4 }, 0.0, 0.0));
    materials["Red"] = 1;

    // Metals
    m_Scene.AddMaterial(RayTracer::Opaque({ 0.9, 0.9, 0.9 }, 0.01, 1));
    materials["Metal"] = 2;

    m_Scene.AddMaterial(RayTracer::Opaque({ 0.9, 0.9, 0.9 }, 0.1, 1));
    materials["Fuzzy metal"] = 3;

    // Clears
    m_Scene.AddMaterial(RayTracer::Clear(1.52, { 0.8, 1, 0.8 }, 0.1));
    materials["Glass"] = 4;

    // Lights
    m_Scene.AddMaterial(RayTracer::Emissive({ 0.1, 0.1, 0.1 }, { 1, 1, 1 }, 20000));
    materials["White light"] = 5;
    
    m_Scene.AddMaterial(RayTracer::Emissive({ 0.1, 0.1, 0.1 }, { 1, 0.5, 0.25 }, 20000));
    materials["Warm light"] = 6;

    // Shiny
    m_Scene.AddMaterial(RayTracer::Opaque({ 0.9, 0.9, 0.9 }, 0.01, 0.5));
    materials["Ceramic"] = 7;
    
    m_Scene.AddMaterial(RayTracer::Opaque({ 0.4, 0.5, 0.5 }, 0.3, 0.2));
    materials["Tiling"] = 8;

    m_Scene.AddObject(RayTracer::Sphere({ 0.0, -100.5, 0.0 }, 100, materials["Tiling"])); // ground
    m_Scene.AddObject(RayTracer::Sphere({ -1.0, 0.0, -2.0 }, 0.5, materials["Red"]));
    m_Scene.AddObject(RayTracer::Sphere({ -1.5, 0, 0 }, 0.5, materials["Ceramic"]));
    m_Scene.AddObject(RayTracer::Sphere({ 1.5, 0, -0.5 }, 0.5, materials["Fuzzy metal"]));
    m_Scene.AddObject(RayTracer::Sphere({ 0, 0, 0 }, 0.5, materials["Glass"]));
    m_Scene.AddObject(RayTracer::Sphere({ -3, 2, 2 }, 1, materials["White light"])); // key light
    m_Scene.AddObject(RayTracer::Sphere({ 3, 150, -150 }, 50, materials["Warm light"])); // back light
}

void AppLayer::OnUpdate(double dt)
{
    bool hasCameraMoved = false;

    GUI::ScreenPosition currentMousePosition = GUI::Input::GetMousePosition();

    if (GUI::Input::IsMouseButtonPressed(2) && currentMousePosition != m_LastMousePosition)
    {
        glfwSetInputMode(GUI::Application::Get().GetWindow().GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL + 2);

        double deltaX = (currentMousePosition.X - m_LastMousePosition.X) * 0.002f;
        double deltaY = (currentMousePosition.Y - m_LastMousePosition.Y) * 0.002f;

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
        ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulation);
        ImGui::DragInt("Depth", &m_Renderer.GetSettings().Depth, 1.0f, 1, 10);
        ImGui::Checkbox("Antialiasing", &m_Renderer.GetSettings().Antialiasing);
        ImGui::Checkbox("Depth of field", &m_Renderer.GetSettings().DepthOfField);
    }

    ImGui::Checkbox("Focus peaking", &m_Renderer.GetSettings().FocusPeaking);

    
    ImGui::InputText("Filename", &m_ExportFilename[0], 255);

    if (ImGui::Button("Export PPM"))
    {
        m_HasUserExported = true;
        m_HasExportSucceeded = EncodePPM(&m_ExportFilename[0], m_ViewportWidth, m_ViewportHeight, m_Renderer.GetImageData());
    }

    if (m_HasUserExported)
    {
        ImGui::Separator();

        if (m_HasExportSucceeded)
            ImGui::Text("Image has been exported !");
        else
            ImGui::Text("Failed to export : file already exists.");
    }

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
    ImGui::Text("Sensivity: %d ASA", m_Camera.GetSensor().Sensivity);
    ImGui::Text("Aperture: f/%.1f", m_Camera.GetLens().Aperture);
    ImGui::Text("Focal length: %.0fmm", m_Camera.GetLens().FocalLength * 1000.0f);
    ImGui::Text("Focus distance: %.0fm", m_Camera.GetLens().FocusDistance);
    ImGui::Text("Neutral density: %.1f", m_Camera.GetNeutralDensity());

    ImGui::DragFloat("Gamma", &m_Camera.GetSensor().Gamma, 0.05f, 0.1f, 1.0f);
    ImGui::DragInt("Sensivity", &m_Camera.GetSensor().Sensivity, 100.0f, 100, 12800);
    ImGui::DragFloat("Aperture", &m_Camera.GetLens().Aperture, 0.1f, 1.0f, 32.0f);
    ImGui::DragFloat("Focal length", &m_Camera.GetLens().FocalLength, 0.005f, 0.014f, 1.0f);
    if (m_Renderer.GetSettings().DepthOfField)
    {
        ImGui::DragFloat("Focus distance", &m_Camera.GetLens().FocusDistance, 0.01f, 0.1f, 100.0f);
    }

    if (ImGui::Button("Add ND"))
    {
        m_Camera.AddNDFilter(0.3);
    }

    if (ImGui::Button("Remove ND"))
    {
        m_Camera.RemoveNDFilter(0.3);
    }

    if (ImGui::Button("Reset camera settings"))
    {
        m_Camera.ResetSettings();
    }

    ImGui::End();

    ImGui::Begin("Scene settings");

    RayTracer::Vector3 unitSkyColor = m_Scene.GetSkyColor().GetUnitVector();
    float skyColorIntensity = static_cast<float>(m_Scene.GetSkyColor().GetLength());

    std::array<float, 3> skyColor = {
            static_cast<float>(unitSkyColor.R),
            static_cast<float>(unitSkyColor.G),
            static_cast<float>(unitSkyColor.B)
    };

    ImGui::ColorEdit3("Sky color", &skyColor[0]);
    ImGui::DragFloat("Sky luminosity", &skyColorIntensity, 1.0f, 1.0f, 1000000.0f);

    m_Scene.r_GetSkyColor().R = skyColor[0];
    m_Scene.r_GetSkyColor().G = skyColor[1];
    m_Scene.r_GetSkyColor().B = skyColor[2];
    m_Scene.r_GetSkyColor() *= skyColorIntensity;

    ImGui::Separator();

    ImGui::Text("Materials");

    for (uint32_t i = 0; i < m_Scene.GetMaterialsCount(); i++)
    {
        ImGui::PushID(i);

        const std::unique_ptr<RayTracer::Material>& material = m_Scene.GetMaterial(i);

        std::array<float, 3> albedo = {
            static_cast<float>(material->GetAlbedo().R),
            static_cast<float>(material->GetAlbedo().G),
            static_cast<float>(material->GetAlbedo().B)
        };

        ImGui::BulletText("Material %d", i);
        ImGui::ColorEdit3("Albedo", &albedo[0]);

        material->r_GetAlbedo().R = albedo[0];
        material->r_GetAlbedo().G = albedo[1];
        material->r_GetAlbedo().B = albedo[2];

        RayTracer::Emissive* emissiveMaterial = dynamic_cast<RayTracer::Emissive*>(material.get());

        if (emissiveMaterial)
        {
            RayTracer::Vector3 unitEmittedColor = emissiveMaterial->GetEmittedColor().GetUnitVector();
            float emittedColorIntensity = static_cast<float>(emissiveMaterial->GetEmittedColor().GetLength());

            std::array<float, 3> emittedColor = {
                static_cast<float>(unitEmittedColor.R),
                static_cast<float>(unitEmittedColor.G),
                static_cast<float>(unitEmittedColor.B)
            };

            ImGui::ColorEdit3("Emitted color", &emittedColor[0]);
            ImGui::DragFloat("Emitted light", &emittedColorIntensity, 1.0f, 1.0f, 1000000.0f);

            emissiveMaterial->r_GetEmittedColor().R = emittedColor[0];
            emissiveMaterial->r_GetEmittedColor().G = emittedColor[1];
            emissiveMaterial->r_GetEmittedColor().B = emittedColor[2];
            emissiveMaterial->r_GetEmittedColor() *= emittedColorIntensity;
        }


        float roughness = static_cast<float>(material->GetRoughness());

        ImGui::DragFloat("Roughness", &roughness, 0.01f, 0.0f, 1.0f);

        material->r_GetRoughness() = roughness;

        RayTracer::Opaque* opaqueMaterial = dynamic_cast<RayTracer::Opaque*>(material.get());

        if (opaqueMaterial)
        {
            float shine = static_cast<float>(opaqueMaterial->GetShine());

            ImGui::DragFloat("Shine", &shine, 0.01f, 0.0f, 1.0f);

            opaqueMaterial->r_GetShine() = shine;
        }
        
        RayTracer::Clear* clearMaterial = dynamic_cast<RayTracer::Clear*>(material.get());

        if (clearMaterial)
        {
            float refractiveIndex = static_cast<float>(clearMaterial->GetRefractiveIndex());

            ImGui::DragFloat("Refractive index", &refractiveIndex, 0.1f, 0.1f, 2.0f);

            clearMaterial->r_GetRefractiveIndex() = refractiveIndex;
        }

        ImGui::PopID();
    }

    ImGui::Separator();

    ImGui::Text("Objects");

    for (uint32_t i = 0; i < m_Scene.GetObjects().size(); i++)
    {
        ImGui::PushID(i);

        const std::unique_ptr<RayTracer::Object>& object = m_Scene.GetObjects()[i];
        RayTracer::Sphere* sphere = dynamic_cast<RayTracer::Sphere*>(object.get());

        if (sphere)
        {
            float sphereX = static_cast<float>(sphere->GetPosition().X);
            float sphereY = static_cast<float>(sphere->GetPosition().Y);
            float sphereZ = static_cast<float>(sphere->GetPosition().Z);
            float sphereRadius = static_cast<float>(sphere->GetRadius());
            int sphereMaterial = static_cast<int>(sphere->GetMaterialIndex());

            ImGui::BulletText("Object %d (sphere)", i);
            ImGui::DragFloat("Position X", &sphereX, 0.1f);
            ImGui::DragFloat("Position Y", &sphereY, 0.1f);
            ImGui::DragFloat("Position Z", &sphereZ, 0.1f);
            ImGui::DragFloat("Radius", &sphereRadius, 0.1f, 0.0f, 1000.0f);
            ImGui::DragInt("Material index", &sphereMaterial, 1.0f, 0, m_Scene.GetMaterialsCount() - 1);

            sphere->r_GetPosition().X = sphereX;
            sphere->r_GetPosition().Y = sphereY;
            sphere->r_GetPosition().Z = sphereZ;
            sphere->r_GetRadius() = sphereRadius;
            sphere->r_GetMaterialIndex() = sphereMaterial;
        }

        ImGui::PopID();
    }

    if (ImGui::Button("Add sphere"))
    {
        m_Scene.AddObject(RayTracer::Sphere({ 0.0, 0.0, 0.0 }, 0.5, 0));
    }

    ImGui::End();

    ImGui::Begin("Viewport");

    m_ViewportWidth = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
    m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

    GUI::Image image = m_Renderer.GetImage();

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