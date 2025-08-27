#pragma once

#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"
#include "Core/Window.h"
#include "Core/ResourceCache.h"
#include "Core/Scene.h"
#include "Core/Entity.h"
#include "Renderer/Camera.h"
#include "Physics/PhysicsSystem.h"
#include "Events/KeyEvent.h"
#include "TLV/TLVReader.h"

#include "Game/Routines/Routine.h"

#include <vector>

namespace Game {

	class MainMenuRoutine : public Routine
	{
	public:
		MainMenuRoutine(PhysicsSystem& ps, const Window& window, MessageBus& bus, Scheduler& scheduler, DefaultCache& resourceCache, const TLVReader& reader);

		MainMenuRoutine(const MainMenuRoutine&) = delete;
		MainMenuRoutine& operator=(const MainMenuRoutine&) = delete;
		MainMenuRoutine(MainMenuRoutine&&) = default;

		Task CreateTask();

		void HandleKeyPress(const KeyEvent& event) override;

	private:
		PhysicsSystem& m_PS;
		const Window& m_Window;
		Scheduler& m_Scheduler;
		DefaultCache& m_ResourceCache;
		std::vector<Entity> m_LevelEntities;
		const TLVReader& m_Reader;
		Camera m_Camera;
		Scene m_Scene;
	};

}
