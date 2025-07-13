#include "Scheduler.h"

#include "Utilities/Error.h"
#include "Utilities/Logger.h"

namespace Game {

	Scheduler::Scheduler()
		: m_Queue{}
		, m_TickCount{}
		, m_Elapsed{}
	{}

	void Scheduler::Add(Task task)
	{
		m_Queue.push_back({ std::move(task), std::nullopt });
	}

	void Scheduler::Reschedule(std::coroutine_handle<> handle, uint32_t waitTicks)
	{
		auto task = std::ranges::find_if(m_Queue, [handle](const auto& e) { return e.task.IsThisTask(handle); });
		Expect(task != std::ranges::end(m_Queue), "Could not find task");

		task->target = waitTicks + m_TickCount;
	}

	void Scheduler::Reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds waitTime)
	{
		auto task = std::ranges::find_if(m_Queue, [handle](const auto& e) { return e.task.IsThisTask(handle); });
		Expect(task != std::ranges::end(m_Queue), "Could not find task");

		task->target = waitTime + m_Elapsed;
	}

	void Scheduler::Run()
	{
		while (!m_Queue.empty())
		{
			const auto start = std::chrono::steady_clock::now();

			for (auto& [task, tickTarget] : m_Queue)
			{
				Expect(task.CanResume(), "Bad task in queue");

				if (tickTarget)
				{
					if (const auto* v_uint = std::get_if<uint32_t>(&*tickTarget); v_uint)
					{
						Expect(*v_uint >= m_TickCount, "Invalid tick target");
						if (*v_uint == m_TickCount)
						{
							task.Resume();
						}
					}
					else if (const auto* v_nano = std::get_if<std::chrono::nanoseconds>(&*tickTarget); v_nano)
					{
						if (m_Elapsed >= *v_nano)
						{
							task.Resume();
						}
					}
				}
				else
				{
					task.Resume();
				}
			}

			std::erase_if(m_Queue, [](const auto& e) { return !e.task.CanResume(); });

			m_TickCount++;
			m_Elapsed += std::chrono::steady_clock::now() - start;
		}
	}

}
