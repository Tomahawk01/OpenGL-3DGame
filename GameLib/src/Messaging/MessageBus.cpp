#include "MessageBus.h"

#include "Utilities/Error.h"
#include "Subscriber.h"

#include <ranges>

namespace {

	template <class... Args>
	auto _PostMessage(Game::MessageType type, auto& subscribers, auto func, Args&&... args)
	{
		for (auto* subscriber : subscribers[type])
		{
			func(subscriber, std::forward<Args>(args)...);
		}
	}

}

namespace Game {

	void MessageBus::Subscribe(MessageType type, Subscriber* subscriber)
	{
		auto& subscribers = m_Subscribers[type];

		Expect(!std::ranges::contains(subscribers, subscriber), "Subscriber already subscribed");

		subscribers.push_back(subscriber);
	}

	void MessageBus::PostKeyPress(const KeyEvent& event)
	{
		_PostMessage(MessageType::KEY_PRESS, m_Subscribers, [](auto* sub, const auto& event) { sub->HandleKeyPress(event); }, event);
	}

	void MessageBus::PostMouseMove(const MouseEvent& event)
	{
		_PostMessage(MessageType::MOUSE_MOVE, m_Subscribers, [](auto* sub, const auto& event) { sub->HandleMouseMove(event); }, event);
	}

}
