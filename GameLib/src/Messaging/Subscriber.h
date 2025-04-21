#pragma once

#include "Utilities/Logger.h"
#include "MessageBus.h"

namespace Game {

	class Subscriber
	{
	public:
		virtual void HandleKeyPress(const KeyEvent&)
		{
			Logger::Warn("Unhandled message!");
		}

		virtual void HandleMouseMove(const MouseEvent&)
		{
			Logger::Warn("Unhandled message!");
		}
	};

}
