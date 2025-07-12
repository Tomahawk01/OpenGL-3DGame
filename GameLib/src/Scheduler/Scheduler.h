#pragma once

#include "Task.h"

#include <deque>
#include <optional>

namespace Game {

	class Scheduler
	{
	public:
		Scheduler();

		void Add(Task task);
		void Reschedule(std::coroutine_handle<> handle, uint32_t waitTicks);
		void Run();

	private:
		struct WaitTask
		{
			Task task;
			std::optional<uint32_t> tickTarget;
		};

		std::deque<WaitTask> m_Queue;
		uint32_t m_TickCount;
	};

}
