#include "SoundRoutine.h"

#include "Utilities/Logger.h"
#include "Scheduler/Wait.h"

namespace Game {

	SoundRoutine::SoundRoutine(MessageBus& bus, Scheduler& scheduler)
		: Routine(bus, {})
		, m_Scheduler{ scheduler }
	{}

	Task SoundRoutine::CreateTask()
	{
		while (m_State != GameState::EXITING)
		{
			co_await Wait{ m_Scheduler, 1u };
		}
	}

}
