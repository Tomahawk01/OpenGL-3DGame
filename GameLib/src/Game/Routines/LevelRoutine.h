#pragma once

#include "Messaging/MessageBus.h"
#include "Messaging/Subscriber.h"
#include "Scheduler/Scheduler.h"
#include "Core/Window.h"
#include "Core/ResourceCache.h"
#include "TLV/TLVReader.h"

#include "Game/Levels/LuaLevel.h"
#include "Game/Player.h"

namespace Game {

	class LevelRoutine : public Subscriber
	{
	public:
		LevelRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler, DefaultCache& resourceCache, const TLVReader& reader);

		LevelRoutine(const LevelRoutine&) = delete;
		LevelRoutine& operator=(const LevelRoutine&) = delete;
		LevelRoutine(LevelRoutine&&) = default;

		Task CreateTask();
		const Player& GetPlayer() const;
		LuaLevel& GetLevel() const;

		void HandleLevelComplete(std::string_view levelName) override;
		void HandleQuit() override;

	private:
		const Window& m_Window;
		MessageBus& m_Bus;
		Scheduler& m_Scheduler;
		Player m_Player;
		DefaultCache& m_ResourceCache;
		const TLVReader& m_Reader;
		size_t m_LevelNum;
		std::unique_ptr<LuaLevel> m_Level;
		bool m_Running;
	};

}
