#pragma once

#include "Task.h"

#include <deque>
#include <functional>
#include <chrono>
#include <memory>

namespace Game {

	class Scheduler
	{
	public:
		Scheduler();

		void Add(Task task);
		void Add(Task task, uint32_t* waitCount);
		void Reschedule(std::coroutine_handle<> handle, uint32_t waitTicks);
		void Reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds waitTime);
		void Reschedule(std::coroutine_handle<> handle, std::unique_ptr<uint32_t> counter);
		void Run();

	private:
		struct WaitTask
		{
			Task task;
			std::move_only_function<bool()> CheckResume;
			uint32_t* parentWaitCount;
		};

		std::deque<WaitTask> m_Queue;
		uint32_t m_TickCount;
		std::chrono::nanoseconds m_Elapsed;
	};

}
