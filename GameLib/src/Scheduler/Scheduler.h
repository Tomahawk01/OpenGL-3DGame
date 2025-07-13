#pragma once

#include "Task.h"

#include <vector>
#include <optional>
#include <chrono>
#include <variant>

namespace Game {

	class Scheduler
	{
	public:
		Scheduler();

		void Add(Task task);
		void Reschedule(std::coroutine_handle<> handle, uint32_t waitTicks);
		void Reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds waitTime);
		void Run();

	private:
		struct WaitTask
		{
			Task task;
			std::optional<std::variant<uint32_t, std::chrono::nanoseconds>> target;
		};

		std::vector<WaitTask> m_Queue;
		uint32_t m_TickCount;
		std::chrono::nanoseconds m_Elapsed;
	};

}
