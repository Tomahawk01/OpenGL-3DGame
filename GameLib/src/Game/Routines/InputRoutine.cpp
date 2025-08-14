#include "InputRoutine.h"

#include "Utilities/Logger.h"
#include "Scheduler/Wait.h"

namespace Game {

	InputRoutine::InputRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler)
		: Routine(bus, {})
		, m_Window(window)
		, m_Scheduler(scheduler)
	{}

	Task InputRoutine::CreateTask()
	{
		while (m_State != GameState::EXITING)
		{
			auto event = m_Window.PollEvent();
			while (event && m_State != GameState::EXITING)
			{
				std::visit(
					[&](auto&& arg)
					{
						using T = std::decay_t<decltype(arg)>;

						if constexpr (std::same_as<T, StopEvent>)
						{
							m_Bus.PostStateChange(GameState::EXITING);
						}
						else if constexpr (std::same_as<T, KeyEvent>)
						{
							m_Bus.PostKeyPress(arg);
						}
						else if constexpr (std::same_as<T, MouseEvent>)
						{
							m_Bus.PostMouseMove(arg);
						}
					}, *event);
				event = m_Window.PollEvent();
			}

			co_await Wait{ m_Scheduler, 1u };
		}
	}

}
