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
		m_Queue.push_back({ std::move(task), nullptr, nullptr });
	}

	void Scheduler::Add(Task task, uint32_t* waitCount)
	{
		m_Queue.push_back({ std::move(task), nullptr, waitCount });
	}

	void Scheduler::Reschedule(std::coroutine_handle<> handle, uint32_t waitTicks)
	{
		auto task = std::ranges::find_if(m_Queue, [handle](const auto& e) { return e.task.IsThisTask(handle); });
		Expect(task != std::ranges::end(m_Queue), "Could not find task");

		const auto futureTick = waitTicks + m_TickCount;

		task->CheckResume = [futureTick, this] {
			Expect(m_TickCount <= futureTick, "Tick count out of sync with await");
			return m_TickCount == futureTick;
		};
	}

	void Scheduler::Reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds waitTime)
	{
		auto task = std::ranges::find_if(m_Queue, [handle](const auto& e) { return e.task.IsThisTask(handle); });
		Expect(task != std::ranges::end(m_Queue), "Could not find task");

		const auto futureTime = waitTime + m_Elapsed;

		task->CheckResume = [futureTime, this] { return m_Elapsed >= futureTime; };
	}

	void Scheduler::Reschedule(std::coroutine_handle<> handle, std::unique_ptr<uint32_t> counter)
	{
		auto task = std::ranges::find_if(m_Queue, [handle](const auto& e) { return e.task.IsThisTask(handle); });
		Expect(task != std::ranges::end(m_Queue), "Could not find task");

		task->CheckResume = [counter = std::move(counter)] { return *counter == 0; };
	}

	void Scheduler::Run()
	{
		while (!m_Queue.empty())
		{
			const auto start = std::chrono::steady_clock::now();

			for (auto& [task, CheckResume, parentCounter] : m_Queue)
			{
				Expect(task.CanResume(), "Bad task in queue");

				if (!CheckResume || CheckResume())
				{
					task.Resume();

					if (parentCounter)
					{
						--(*parentCounter);
					}
				}
			}

			std::erase_if(m_Queue, [](const auto& e) { return !e.task.CanResume(); });

			m_TickCount++;
			m_Elapsed += std::chrono::steady_clock::now() - start;
		}
	}

}
