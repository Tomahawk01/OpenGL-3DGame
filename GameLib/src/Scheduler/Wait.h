#pragma once

#include "Scheduler.h"

#include <coroutine>
#include <cstdint>
#include <memory>

namespace Game {

	template <class T>
	class Wait
	{
	public:
		Wait(Scheduler& scheduler, T waitObject)
			: m_WaitObject{ std::move(waitObject) }
			, m_Scheduler{ scheduler }
		{}

		bool await_ready()
		{
			return false;
		}

		auto await_suspend(std::coroutine_handle<> h)
		{
			if constexpr (std::same_as<T, Task>)
			{
				auto counter = std::make_unique<uint32_t>(1u);
				m_Scheduler.Add(std::move(m_WaitObject), counter.get());
				m_Scheduler.Reschedule(h, std::move(counter));
			}
			else
			{
				m_Scheduler.Reschedule(h, m_WaitObject);
			}
		}

		auto await_resume()
		{
		}

	private:
		T m_WaitObject;
		Scheduler& m_Scheduler;
	};

}
