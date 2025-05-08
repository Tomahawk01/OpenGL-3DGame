#pragma once

#include "Key.h"

#include <string>

namespace Game {

	class KeyEvent
	{
	public:
		KeyEvent(Key key, KeyState state);

		Key GetKey() const;
		KeyState GetState() const;

		bool operator==(const KeyEvent&) const = default;

		std::string to_string() const;

	private:
		Key m_Key;
		KeyState m_State;
	};

}
