#include "KeyEvent.h"

namespace Game {

	KeyEvent::KeyEvent(Key key, KeyState state)
		: m_Key(key)
		, m_State(state)
	{}

	Key KeyEvent::GetKey() const
	{
		return m_Key;
	}

	KeyState KeyEvent::GetState() const
	{
		return m_State;
	}

}
