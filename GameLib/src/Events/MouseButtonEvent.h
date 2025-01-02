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
		MouseButtonEvent(float x, float y, MouseButtonState state);

		float GetX() const;
		float GetY() const;
		MouseButtonState GetState() const;

	private:
		float m_X;
		float m_Y;
		MouseButtonState m_State;
	};

}

template<>
struct std::formatter<Game::MouseButtonState>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::MouseButtonState& obj, std::format_context& ctx) const
	{
		switch (obj)
		{
		case Game::MouseButtonState::UP:
			return std::format_to(ctx.out(), "UP");
		case Game::MouseButtonState::DOWN:
			return std::format_to(ctx.out(), "DOWN");
		default:
			return std::format_to(ctx.out(), "UNKNOWN");
		}
	}
};

template<>
struct std::formatter<Game::MouseButtonEvent>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::MouseButtonEvent& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "MouseButtonEvent {} {} {}", obj.GetState(), obj.GetX(), obj.GetY());
	}
};
