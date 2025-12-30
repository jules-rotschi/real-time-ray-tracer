#pragma once

namespace GUI
{
	struct ScreenPosition
	{
		double X = 0;
		double Y = 0;

		friend bool operator==(ScreenPosition lhs, ScreenPosition rhs) = default;
	};

	namespace Input
	{
		bool IsKeyDown(char key);
		bool IsMouseButtonPressed(int button);
		ScreenPosition GetMousePosition();
	}
}