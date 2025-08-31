#pragma once

#include "Game/Routines/Routine.h"
#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"
#include "Core/Window.h"

namespace Game {

	class SoundRoutine : public Routine
	{
	public:
		SoundRoutine(MessageBus& bus, Scheduler& scheduler);

		~SoundRoutine() override = default;
		SoundRoutine(const SoundRoutine&) = delete;
		SoundRoutine& operator=(const SoundRoutine&) = delete;
		SoundRoutine(SoundRoutine&&) = default;

		Task CreateTask();

	private:
		Scheduler& m_Scheduler;
	};

}
