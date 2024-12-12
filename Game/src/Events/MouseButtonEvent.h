#pragma once

#include <format>

namespace Game {

	enum class MouseButtonState
	{
		UP,
		DOWN
	};

	class MouseButtonEvent
	{
	public:
		MouseButtonEvent(float x, float y, MouseButtonState state)
			: m_X(x)
			, m_Y(y)
			, m_State(state)
		{}

		float GetX() const { return m_X; }
		float GetY() const { return m_Y; }
		MouseButtonState GetState() const { return m_State; }

	private:
		float m_X;
		float m_Y;
		MouseButtonState m_State;
	};

}

// TODO: Add formatter

//template<>
//struct std::formatter<Game::MouseButtonEvent>
//{
//	constexpr auto parse(std::format_parse_context& ctx)
//	{
//		return std::begin(ctx);
//	}
//
//	auto format(const Game::MouseButtonEvent& obj, std::format_context& ctx) const
//	{
//		return std::format_to(ctx.out(), "MouseButtonEvent {} {}", obj.GetX(), obj.GetY());
//	}
//};
