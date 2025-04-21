#pragma once

#include "Utilities/Logger.h"
#include "MessageBus.h"

namespace Game {

	class Subscriber
	{
	public:
		template <class T>
		static void Subscribe(T* obj, MessageBus& bus)
		{
			if (&T::HandleKeyPress != &HandleKeyPress)
			{
				bus.Subscribe(MessageType::KEY_PRESS, obj);
			}
			if (&T::HandleMouseMove != &HandleMouseMove)
			{
				bus.Subscribe(MessageType::MOUSE_MOVE, obj);
			}
		}

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
