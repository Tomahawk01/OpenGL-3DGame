#pragma once

#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"
#include "Core/Window.h"
#include "Core/ResourceCache.h"
#include "Core/Scene.h"
#include "Renderer/Camera.h"
#include "Events/KeyEvent.h"
#include "TLV/TLVReader.h"

#include "Game/Routines/Routine.h"

namespace Game {

	class MainMenuRoutine : public Routine
	{
	public:
		MainMenuRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler, DefaultCache& resourceCache, const TLVReader& reader);

		MainMenuRoutine(const MainMenuRoutine&) = delete;
		MainMenuRoutine& operator=(const MainMenuRoutine&) = delete;
		MainMenuRoutine(MainMenuRoutine&&) = default;

		Task CreateTask();

		void HandleKeyPress(const KeyEvent& event) override;

	private:
		const Window& m_Window;
		Scheduler& m_Scheduler;
		DefaultCache& m_ResourceCache;
		const TLVReader& m_Reader;
		Camera m_Camera;
		Scene m_Scene;
	};

}
