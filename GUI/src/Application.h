#pragma once

#include <string>
#include <vector>
#include <memory>

#include "Window.h"
#include "Layer.h"

namespace GUI
{
	struct ApplicationSpecification
	{
		std::string Name = "Application";
		WindowSpecification WindowSpecification;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& specification);
		~Application();

		void Run() const;

		template<typename TLayer>
		requires(std::is_base_of_v<Layer, TLayer>)
		void PushLayer()
		{
			m_LayerStack.push_back(std::make_unique<TLayer>());
		}

		const Window& GetWindow() const;

		static Application& Get();
		static float GetTime();

	private:
		ApplicationSpecification m_Specification;
		Window m_Window;
		std::vector<std::unique_ptr<Layer>> m_LayerStack;
	};
}