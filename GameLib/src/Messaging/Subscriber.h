#pragma once

#include "Utilities/Logger.h"
#include "Events/KeyEvent.h"
#include "MessageBus.h"

namespace Game {

	class Subscriber
	{
	public:
		virtual void HandleKeyPress(const KeyEvent&)
		{
			Logger::Warn("Unhandled message!");
		}
	};

}
