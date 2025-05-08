#include "MouseButtonEvent.h"

#include "Utilities/Formatter.h"

namespace Game {

	MouseButtonEvent::MouseButtonEvent(float x, float y, MouseButtonState state)
		: m_X(x)
		, m_Y(y)
		, m_State(state)
	{}

	float MouseButtonEvent::GetX() const
	{
		return m_X;
	}

	float MouseButtonEvent::GetY() const
	{
		return m_Y;
	}

	MouseButtonState MouseButtonEvent::GetState() const
	{
		return m_State;
	}

	std::string MouseButtonEvent::to_string() const
	{
		return std::format("MouseButtonEvent {} {} {}", GetState(), GetX(), GetY());
	}

	std::string to_string(MouseButtonState obj)
	{
		switch (obj)
		{
		case MouseButtonState::UP: return "UP";
		case MouseButtonState::DOWN: return "DOWN";
		default: return "UNKNOWN";
		}
	}

}
