#pragma once

#include "Scheduler.h"

#include <coroutine>
#include <cstdint>

namespace Game {

	struct Wait
	{
		Wait(Scheduler& scheduler, uint32_t ticks)
			: WaitTicks(ticks)
			, Scheduler{ scheduler }
		{
		}

		bool await_ready()
		{
			return false;
		}

		auto await_suspend(std::coroutine_handle<> h)
		{
			Scheduler.Reschedule(h, WaitTicks);
		}

		auto await_resume()
		{
		}

		uint32_t WaitTicks;
		Scheduler& Scheduler;
	};

}
