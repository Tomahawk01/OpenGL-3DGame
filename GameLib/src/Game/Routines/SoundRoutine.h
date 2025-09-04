#pragma once

#include "Core/ResourceCache.h"
#include "Game/Routines/Routine.h"
#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"

#include <memory>

namespace Game {

	class SoundRoutine : public Routine
	{
	public:
		SoundRoutine(MessageBus& bus, Scheduler& scheduler, DefaultCache& resourceCache);

		~SoundRoutine() override;
		SoundRoutine(const SoundRoutine&) = delete;
		SoundRoutine& operator=(const SoundRoutine&) = delete;
		SoundRoutine(SoundRoutine&&) = default;

		Task CreateTask();

	private:
		struct Implementation;
		std::unique_ptr<Implementation> m_Impl;
		Scheduler& m_Scheduler;
		DefaultCache& m_ResourceCache;
	};

}
