#include "InputRoutine.h"

#include "Scheduler/Wait.h"

namespace Game {

	InputRoutine::InputRoutine(const Window& window, MessageBus& bus, Scheduler& scheduler)
		: m_Window(window)
		, m_Bus(bus)
		, m_Scheduler(scheduler)
	{}

	Task InputRoutine::CreateTask()
	{
		bool running = true;
		while (running)
		{
			auto event = m_Window.PollEvent();
			while (event && running)
			{
				std::visit(
					[&](auto&& arg)
					{
						using T = std::decay_t<decltype(arg)>;

						if constexpr (std::same_as<T, StopEvent>)
						{
							running = false;
						}
						else if constexpr (std::same_as<T, KeyEvent>)
						{
							if (arg.GetKey() == Key::ESC)
							{
								running = false;
							}

							m_Bus.PostKeyPress(arg);
						}
						else if constexpr (std::same_as<T, MouseEvent>)
						{
							m_Bus.PostMouseMove(arg);
						}
					}, *event);
				event = m_Window.PollEvent();
			}

			if (running)
			{
				co_await Wait{ m_Scheduler, 1u };
			}
			else
			{
				m_Bus.PostQuit();
			}
		}
	}

}
