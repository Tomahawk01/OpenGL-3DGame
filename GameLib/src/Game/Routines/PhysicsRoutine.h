#pragma once

#include "Routine.h"
#include "Messaging/MessageBus.h"
#include "Scheduler/Scheduler.h"
#include "Physics/PhysicsSystem.h"

namespace Game {

	class PhysicsRoutine : public Routine
	{
	public:
		PhysicsRoutine(PhysicsSystem& ps, MessageBus& bus, Scheduler& scheduler);

		~PhysicsRoutine() override = default;
		PhysicsRoutine(const PhysicsRoutine&) = delete;
		PhysicsRoutine& operator=(const PhysicsRoutine&) = delete;
		PhysicsRoutine(PhysicsRoutine&&) = default;

		Task CreateTask();

	private:
		PhysicsSystem& m_PS;
		Scheduler& m_Scheduler;
	};

}
