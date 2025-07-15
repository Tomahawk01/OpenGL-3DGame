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

		virtual void HandleLevelComplete(std::string_view)
		{
			Logger::Warn("Unhandled message!");
		}

		virtual void HandleEntityIntersect(const Entity*, const Entity*)
		{
			Logger::Warn("Unhandled message!");
		}

		virtual void HandleRestartLevel()
		{
			Logger::Warn("Unhandled message!");
		}

		virtual void HandleQuit()
		{
			Logger::Warn("Unhandled message!");
		}
	};

}
