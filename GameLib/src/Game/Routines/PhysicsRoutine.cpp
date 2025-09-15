#include "PhysicsRoutine.h"

#include "Scheduler/Wait.h"
#include "Game/GameState.h"

using namespace std::literals;

namespace Game {

	PhysicsRoutine::PhysicsRoutine(PhysicsSystem& ps, MessageBus& bus, Scheduler& scheduler)
		: Routine(bus, {})
		, m_PS{ ps }
		, m_Scheduler{ scheduler }
	{}

	Task PhysicsRoutine::CreateTask()
	{
		while (m_State != GameState::EXITING)
		{
			if (m_State != GameState::RUNNING)
			{
				co_await Wait{ m_Scheduler, GameState::RUNNING };
			}

			m_PS.Update();

			co_await Wait{ m_Scheduler, 16ms };
		}
	}

}
