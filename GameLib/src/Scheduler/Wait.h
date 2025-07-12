#pragma once

#include "Scheduler.h"

#include <coroutine>
#include <cstdint>
#include <chrono>
#include <variant>

namespace Game {

	struct Wait
	{
		Wait(Scheduler& scheduler, uint32_t waitTicks)
			: WaitObject{ waitTicks }
			, Scheduler{ scheduler }
		{}

		Wait(Scheduler& scheduler, std::chrono::nanoseconds waitTime)
			: WaitObject{ waitTime }
			, Scheduler{ scheduler }
		{}

		bool await_ready()
		{
			return false;
		}

		auto await_suspend(std::coroutine_handle<> h)
		{
			if (const auto* v_uint = std::get_if<uint32_t>(&WaitObject); v_uint)
			{
				Scheduler.Reschedule(h, *v_uint);
			}
			else if (const auto* v_nano = std::get_if<std::chrono::nanoseconds>(&WaitObject); v_nano)
			{
				Scheduler.Reschedule(h, *v_nano);
			}
		}

		auto await_resume()
		{
		}

		std::variant<uint32_t, std::chrono::nanoseconds> WaitObject;
		Scheduler& Scheduler;
	};

}
