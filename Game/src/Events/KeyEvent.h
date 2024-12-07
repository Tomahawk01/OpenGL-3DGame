#pragma once

#include "Key.h"

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
