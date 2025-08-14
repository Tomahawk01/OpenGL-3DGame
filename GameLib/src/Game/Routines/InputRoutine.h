#pragma once

#include "Game/Routines/Routine.h"
#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"
#include "Core/Window.h"

namespace Game {

	class InputRoutine : public Routine
	{
	public:
		InputRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler);

		~InputRoutine() override = default;
		InputRoutine(const InputRoutine&) = delete;
		InputRoutine& operator=(const InputRoutine&) = delete;
		InputRoutine(InputRoutine&&) = default;

		Task CreateTask();

	private:
		const Window& m_Window;
		Scheduler& m_Scheduler;
	};

}
