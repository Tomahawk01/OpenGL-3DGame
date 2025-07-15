#pragma once

#include "Messaging/Subscriber.h"
#include "Core/Window.h"

#include <string_view>
#include <memory>

namespace Game {

	class Game : public Subscriber
	{
	public:
		Game(const std::vector<std::string_view>& args);

		void Run(std::string_view resourceRoot);

	private:
		bool m_Running;
		MessageBus m_Bus;
		Window m_Window;
	};

}
