#pragma once

#include "Task.h"
#include "Messaging/MessageBus.h"
#include "Messaging/Subscriber.h"
#include "Messaging/AutoSubscribe.h"
#include "Game/GameState.h"

#include <deque>
#include <functional>
#include <chrono>
#include <memory>

namespace Game {

	class Scheduler : public Subscriber
	{
	public:
		Scheduler(MessageBus& bus);
		~Scheduler() override = default;

		void Add(Task task);
		void Add(Task task, uint32_t* waitCount);
		void Reschedule(std::coroutine_handle<> handle, uint32_t waitTicks);
		void Reschedule(std::coroutine_handle<> handle, std::chrono::nanoseconds waitTime);
		void Reschedule(std::coroutine_handle<> handle, std::unique_ptr<uint32_t> counter);
		void Reschedule(std::coroutine_handle<> handle, GameState state);
		void Run();

		void HandleStateChange(GameState state) override;

	private:
		struct WaitTask
		{
			Task task;
			std::move_only_function<bool()> CheckResume;
			uint32_t* parentWaitCount;
		};

		MessageBus& m_Bus;
		AutoSubscribe m_AutoSub;
		std::deque<WaitTask> m_Queue;
		uint32_t m_TickCount;
		std::chrono::nanoseconds m_Elapsed;
		GameState m_State;
		GameState m_NextState;
	};

}
