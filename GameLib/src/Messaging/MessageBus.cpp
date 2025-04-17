#include "MessageBus.h"

#include "Utilities/Error.h"
#include "Subscriber.h"

#include <ranges>

namespace Game {

	void MessageBus::Subscribe(MessageType type, Subscriber* subscriber)
	{
		auto& subscribers = m_Subscribers[type];

		Expect(!std::ranges::contains(subscribers, subscriber), "Subscriber already subscribed");

		subscribers.push_back(subscriber);
	}

	void MessageBus::PostKeyPress(const KeyEvent& event)
	{
		for (auto* subscriber : m_Subscribers[MessageType::KEY_PRESS])
		{
			subscriber->HandleKeyPress(event);
		}
	}

}
