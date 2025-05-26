#pragma once

#include "Messaging/Subscriber.h"
#include "Core/Window.h"

#include "Game/Levels/LuaLevel.h"
#include "Game/Player.h"

#include <string_view>
#include <memory>

namespace Game {

	class Game : public Subscriber
	{
	public:
		Game();

		void Run(std::string_view resourceRoot);

		void HandleLevelComplete(std::string_view levelName) override;

	private:
		bool m_Running;
		std::vector<std::unique_ptr<LuaLevel>> m_Levels;
		size_t m_LevelNum;
		MessageBus m_Bus;
		Window m_Window;
		Player m_Player;
	};

}
