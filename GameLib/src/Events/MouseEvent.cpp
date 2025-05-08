#include "MouseEvent.h"

#include <format>

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

	std::string MouseEvent::to_string() const
	{
		return std::format("MouseEvent {} {}", GetDeltaX(), GetDeltaY());
	}

}
