#pragma once

#include "Events/KeyEvent.h"
#include "MessageBus.h"

namespace Game {

	class Subscriber
	{
	public:
		virtual void HandleKeyPress(const KeyEvent&) {};

	private:

	};

}
