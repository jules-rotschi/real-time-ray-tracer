#pragma once

#include <memory>

namespace GUI
{
	class Layer
	{
	public:
		virtual ~Layer() = default;

		virtual void OnUpdate(double dt) { /*optionnally implement in subclasses*/ }
		virtual void OnRender() { /*optionnally implement in subclasses*/ }
	};
}