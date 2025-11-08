#pragma once

#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"
#include "Scripting/ScriptLoader.h"
#include "Core/Window.h"
#include "Core/ResourceCache.h"
#include "Physics/PhysicsSystem.h"
#include "Events/KeyEvent.h"
#include "TLV/TLVReader.h"

#include "Game/Levels/LuaLevel.h"
#include "Game/Routines/Routine.h"
#include "Game/Player.h"

namespace Game {

	class LevelRoutine : public Routine
	{
	public:
		LevelRoutine(PhysicsSystem& ps, Window& window, MessageBus& bus, Scheduler& scheduler, DefaultCache& resourceCache, const TLVReader& reader);

		LevelRoutine(const LevelRoutine&) = delete;
		LevelRoutine& operator=(const LevelRoutine&) = delete;
		LevelRoutine(LevelRoutine&&) = default;

		Task CreateTask();
		const Player& GetPlayer() const;
		LuaLevel& GetLevel() const;

		void HandleLevelComplete(std::string_view levelName) override;
		void HandleKeyPress(const KeyEvent& event) override;
		void HandleMouseMove(const MouseEvent& event) override;

	private:
		PhysicsSystem& m_PS;
		Window& m_Window;
		Scheduler& m_Scheduler;
		Player m_Player;
		DefaultCache& m_ResourceCache;
		const TLVReader& m_Reader;
		size_t m_LevelNum;
		std::vector<ScriptLoader> m_LevelLoaders;
		std::unique_ptr<LuaLevel> m_Level;
	};

}
