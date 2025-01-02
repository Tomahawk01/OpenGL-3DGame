#include "MouseButtonEvent.h"

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

}
