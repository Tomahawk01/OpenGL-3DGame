#include "MouseEvent.h"

namespace Game {

	MouseEvent::MouseEvent(float deltaX, float deltaY)
		: m_DeltaX(deltaX)
		, m_DeltaY(deltaY)
	{}

	float MouseEvent::GetDeltaX() const
	{
		return m_DeltaX;
	}

	float MouseEvent::GetDeltaY() const
	{
		return m_DeltaY;
	}

}
