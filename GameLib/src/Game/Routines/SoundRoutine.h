#pragma once

#include "Game/Routines/Routine.h"
#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"
#include "Core/Window.h"

#include <memory>

namespace Game {

	class SoundRoutine : public Routine
	{
	public:
		SoundRoutine(MessageBus& bus, Scheduler& scheduler);

		~SoundRoutine() override;
		SoundRoutine(const SoundRoutine&) = delete;
		SoundRoutine& operator=(const SoundRoutine&) = delete;
		SoundRoutine(SoundRoutine&&) = default;

		Task CreateTask();

	private:
		struct implementation;
		std::unique_ptr<implementation> m_Impl;
		Scheduler& m_Scheduler;
	};

}
