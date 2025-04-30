#pragma once

#include "Messaging/Subscriber.h"

#include <string_view>

namespace Game {

	class Game : public Subscriber
	{
	public:
		Game();

		void Run(std::string_view resourceRoot);

		void HandleLevelComplete(std::string_view levelName) override;

	private:
		bool m_Running;
	};

}
