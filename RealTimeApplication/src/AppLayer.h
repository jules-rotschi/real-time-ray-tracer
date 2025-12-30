#pragma once

#include <array>

#include "Layer.h"
#include "Input/Input.h"

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"

class AppLayer : public GUI::Layer
{
public:
	AppLayer();

	void OnUpdate(double dt) override;
	void OnRender() override;

private:
	void Render();

private:
	RayTracer::Renderer m_Renderer;
	RayTracer::Scene m_Scene;
	RayTracer::Camera m_Camera;

	uint32_t m_ViewportWidth = 0;
	uint32_t m_ViewportHeight = 0;

	double m_LastRenderTime = 0.0;

	bool m_Render = true;

	char m_ExportFilename[255] = "image.ppm";
	bool m_HasUserExported = false;
	bool m_HasExportSucceeded = false;

	GUI::ScreenPosition m_LastMousePosition;
};