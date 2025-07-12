#include "Scheduler.h"

#include "Utilities/Error.h"
#include "Utilities/Logger.h"

namespace Game {

	Scheduler::Scheduler()
		: m_Queue{}
		, m_TickCount{}
	{}

	void Scheduler::Add(Task task)
	{
		m_Queue.push_front({ std::move(task), std::nullopt });
	}

	void Scheduler::Reschedule(std::coroutine_handle<> handle, uint32_t waitTicks)
	{
		auto task = std::ranges::find_if(m_Queue, [handle](const auto& e) { return e.task.IsThisTask(handle); });
		Expect(task != std::ranges::end(m_Queue), "Could not find task");

		task->tickTarget = waitTicks + m_TickCount;
	}

	void Scheduler::Run()
	{
		while (!m_Queue.empty())
		{
			for (auto& [task, tickTarget] : m_Queue)
			{
				Expect(task.CanResume(), "Bad task in queue");

				if (tickTarget)
				{
					Expect(*tickTarget >= m_TickCount, "Invalid tick target");
					if (*tickTarget == m_TickCount)
					{
						task.Resume();
					}
				}
				else
				{
					task.Resume();
				}
			}

			m_Queue.erase(
				std::remove_if(std::begin(m_Queue), std::end(m_Queue), [](const auto& e) { return !e.task.CanResume(); }),
				std::end(m_Queue));

			m_TickCount++;
		}
	}

}
