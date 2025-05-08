#pragma once

#include <string>

namespace Game {

	class MouseEvent
	{
	public:
		MouseEvent(float deltaX, float deltaY);

		float GetDeltaX() const;
		float GetDeltaY() const;

		std::string to_string() const;

	private:
		float m_DeltaX;
		float m_DeltaY;
	};

}
