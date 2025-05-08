#pragma once

#include <Windows.h>

#include <string>

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

		SPACE = VK_SPACE,

		F1 =	VK_F1,
	};

	enum class KeyState
	{
		UP,
		DOWN
	};

	inline std::string to_string(Key obj)
	{
		switch (obj)
		{
		case Key::ESC: return "ESC";
		case Key::A: return "A";
		case Key::B: return "B";
		case Key::C: return "C";
		case Key::D: return "D";
		case Key::E: return "E";
		case Key::F: return "F";
		case Key::G: return "G";
		case Key::H: return "H";
		case Key::I: return "I";
		case Key::J: return "J";
		case Key::K: return "K";
		case Key::L: return "L";
		case Key::M: return "M";
		case Key::N: return "N";
		case Key::O: return "O";
		case Key::P: return "P";
		case Key::Q: return "Q";
		case Key::R: return "R";
		case Key::S: return "S";
		case Key::T: return "T";
		case Key::U: return "U";
		case Key::V: return "V";
		case Key::W: return "W";
		case Key::X: return "X";
		case Key::Y: return "Y";
		case Key::Z: return "Z";
		case Key::SPACE: return "SPACE";
		case Key::F1: return "F1";
		}

		return "?";
	}

	inline std::string to_string(KeyState obj)
	{
		switch (obj)
		{
		case KeyState::UP: return "UP";
		case KeyState::DOWN: return "DOWN";
		default: return "UNKNOWN STATE";
		}
	}

}
