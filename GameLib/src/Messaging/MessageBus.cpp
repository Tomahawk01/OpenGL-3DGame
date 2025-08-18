#include "MessageBus.h"

#include "Utilities/Error.h"
#include "Subscriber.h"
#include "Renderer/Camera.h"
#include "Core/Scene.h"

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

	void MessageBus::Unsubscribe(MessageType type, Subscriber* subscriber)
	{
		auto& subscribers = m_Subscribers[type];

		Expect(std::ranges::contains(subscribers, subscriber), "Subscriber not subscribed");

		std::erase(subscribers, subscriber);
	}

	void MessageBus::PostKeyPress(const KeyEvent& event)
	{
		_PostMessage(MessageType::KEY_PRESS, m_Subscribers, [](auto* sub, const auto& event) { sub->HandleKeyPress(event); }, event);
	}

	void MessageBus::PostMouseMove(const MouseEvent& event)
	{
		_PostMessage(MessageType::MOUSE_MOVE, m_Subscribers, [](auto* sub, const auto& event) { sub->HandleMouseMove(event); }, event);
	}

	void MessageBus::PostLevelComplete(std::string_view levelName)
	{
		_PostMessage(MessageType::LEVEL_COMPLETE, m_Subscribers, [](auto* sub, const auto& levelName) { sub->HandleLevelComplete(levelName); }, levelName);
	}

	void MessageBus::PostEntityIntersect(const Entity* a, const Entity* b)
	{
		_PostMessage(MessageType::ENTITY_INTERSECT, m_Subscribers, [](auto* sub, const auto* a, const auto* b) { sub->HandleEntityIntersect(a, b); }, a, b);
	}

	void MessageBus::PostRestartLevel()
	{
		_PostMessage(MessageType::RESTART_LEVEL, m_Subscribers, [](auto* sub) { sub->HandleRestartLevel(); });
	}

	void MessageBus::PostQuit()
	{
		_PostMessage(MessageType::QUIT, m_Subscribers, [](auto* sub) { sub->HandleQuit(); });
	}

	void MessageBus::PostStateChange(GameState state)
	{
		_PostMessage(MessageType::STATE_CHANGE, m_Subscribers, [state](auto* sub) { sub->HandleStateChange(state); });
	}

	void MessageBus::PostNewCamera(Camera* camera)
	{
		_PostMessage(MessageType::NEW_CAMERA, m_Subscribers, [camera](auto* sub) { sub->HandleNewCamera(camera); });
	}

	void MessageBus::PostNewScene(Scene* scene)
	{
		_PostMessage(MessageType::NEW_SCENE, m_Subscribers, [scene](auto* sub) { sub->HandleNewScene(scene); });
	}

}
