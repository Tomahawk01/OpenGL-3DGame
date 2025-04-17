#pragma once

#include "Events/KeyEvent.h"

#include <any>
#include <unordered_map>
#include <vector>

namespace Game {

	class Subscriber;

	enum class MessageType
	{
		KEY_PRESS,
	};

	class MessageBus
	{
	public:
		void Subscribe(MessageType type, Subscriber* subscriber);

		void PostKeyPress(const KeyEvent& event);

	private:
		std::unordered_map<MessageType, std::vector<Subscriber*>> m_Subscribers;
	};

}
