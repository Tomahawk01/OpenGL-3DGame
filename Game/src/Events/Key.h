#pragma once

#include <Windows.h>

#include <string_view>
#include <format>

using namespace std::literals;

namespace Game {

	enum class Key
	{
		ESC =   0x1B,

		A =     0x41,
		B =     0x42,
		C =     0x43,
		D =     0x44,
		E =     0x45,
		F =     0x46,
		G =     0x47,
		H =     0x48,
		I =     0x49,
		J =     0x4A,
		K =     0x4B,
		L =     0x4C,
		M =     0x4D,
		N =     0x4E,
		O =     0x4F,
		P =     0x50,
		Q =     0x51,
		R =     0x52,
		S =     0x53,
		T =     0x54,
		U =     0x55,
		V =     0x56,
		W =     0x57,
		X =     0x58,
		Y =     0x59,
		Z =     0x5A,

		F1 =	VK_F1,
	};

	enum class KeyState
	{
		UP,
		DOWN
	};

}

template<>
struct std::formatter<Game::Key>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::Key& obj, std::format_context& ctx) const
	{
		std::string_view s{ "?"sv };

		switch (obj)
		{
		case Game::Key::ESC: s = "ESC"sv; break;
		case Game::Key::A: s = "A"sv; break;
		case Game::Key::B: s = "B"sv; break;
		case Game::Key::C: s = "C"sv; break;
		case Game::Key::D: s = "D"sv; break;
		case Game::Key::E: s = "E"sv; break;
		case Game::Key::F: s = "F"sv; break;
		case Game::Key::G: s = "G"sv; break;
		case Game::Key::H: s = "H"sv; break;
		case Game::Key::I: s = "I"sv; break;
		case Game::Key::J: s = "J"sv; break;
		case Game::Key::K: s = "K"sv; break;
		case Game::Key::L: s = "L"sv; break;
		case Game::Key::M: s = "M"sv; break;
		case Game::Key::N: s = "N"sv; break;
		case Game::Key::O: s = "O"sv; break;
		case Game::Key::P: s = "P"sv; break;
		case Game::Key::Q: s = "Q"sv; break;
		case Game::Key::R: s = "R"sv; break;
		case Game::Key::S: s = "S"sv; break;
		case Game::Key::T: s = "T"sv; break;
		case Game::Key::U: s = "U"sv; break;
		case Game::Key::V: s = "V"sv; break;
		case Game::Key::W: s = "W"sv; break;
		case Game::Key::X: s = "X"sv; break;
		case Game::Key::Y: s = "Y"sv; break;
		case Game::Key::Z: s = "Z"sv; break;
		}

		return std::format_to(ctx.out(), "{}", s);
	}
};

template<>
struct std::formatter<Game::KeyState>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return std::begin(ctx);
	}

	auto format(const Game::KeyState& obj, std::format_context& ctx) const
	{
		switch (obj)
		{
		case Game::KeyState::UP: return std::format_to(ctx.out(), "UP");
		case Game::KeyState::DOWN: return std::format_to(ctx.out(), "DOWN");
		default: return std::format_to(ctx.out(), "UNKNOWN STATE");
		}
	}
};
