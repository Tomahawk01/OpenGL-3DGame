#pragma once

#include "Scheduler.h"

#include <coroutine>
#include <cstdint>

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
			m_Scheduler.Reschedule(h, m_WaitObject);
		}

		auto await_resume()
		{
		}

	private:
		T m_WaitObject;
		Scheduler& m_Scheduler;
	};

}
