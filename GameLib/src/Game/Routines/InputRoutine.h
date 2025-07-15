#pragma once

#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"
#include "Core/Window.h"

namespace Game {

	class InputRoutine
	{
	public:
		InputRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler);

		InputRoutine(const InputRoutine&) = delete;
		InputRoutine& operator=(const InputRoutine&) = delete;
		InputRoutine(InputRoutine&&) = default;

		Task CreateTask();

	private:
		const Window& m_Window;
		MessageBus& m_Bus;
		Scheduler& m_Scheduler;
	};

}
