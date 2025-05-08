#pragma once

#include <string>

namespace Game {

	enum class MouseButtonState
	{
		UP,
		DOWN
	};

	class MouseButtonEvent
	{
	public:
		MouseButtonEvent(float x, float y, MouseButtonState state);

		float GetX() const;
		float GetY() const;
		MouseButtonState GetState() const;

		std::string to_string() const;

	private:
		float m_X;
		float m_Y;
		MouseButtonState m_State;
	};

	std::string to_string(MouseButtonState obj);

}
