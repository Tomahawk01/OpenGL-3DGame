#pragma once

#include "Key.h"

#include <format>

namespace Game {

	class KeyEvent
	{
	public:
		KeyEvent(Key key, KeyState state);

		Key GetKey() const;
		KeyState GetState() const;

	private:
		Key m_Key;
		KeyState m_State;
	};

}

template<>
struct std::formatter<Game::KeyEvent>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::KeyEvent& obj, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "KeyEvent {} {}", obj.GetKey(), obj.GetState());
	}
};
