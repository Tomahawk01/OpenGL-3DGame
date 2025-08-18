#pragma once

#include <string>

namespace Game {

	enum class GameState
	{
		MAIN_MENU,
		RUNNING,
		PAUSED,
		EXITING
	};

	inline std::string to_string(GameState state)
	{
		switch (state)
		{
			case GameState::MAIN_MENU: return "MAIN_MENU"; break;
			case GameState::RUNNING: return "RUNNING"; break;
			case GameState::PAUSED: return "PAUSED"; break;
			case GameState::EXITING: return "EXITING"; break;
		}

		return "UNKNOWN";
	}

}
