#pragma once

#include <format>

namespace Game {

	class MouseEvent
	{
	public:
		MouseEvent(float deltaX, float deltaY);

		float GetDeltaX() const;
		float GetDeltaY() const;

	private:
		float m_DeltaX;
		float m_DeltaY;
	};

}

template<>
struct std::formatter<Game::MouseEvent>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::MouseEvent& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "MouseEvent {} {}", obj.GetDeltaX(), obj.GetDeltaY());
	}
};
