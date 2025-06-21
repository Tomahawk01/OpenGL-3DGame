#pragma once

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include <string_view>
#include <unordered_map>
#include <vector>

namespace Game {

	class Subscriber;
	class Entity;

	enum class MessageType
	{
		KEY_PRESS,
		MOUSE_MOVE,
		LEVEL_COMPLETE,
		ENTITY_INTERSECT,
		RESTART_LEVEL
	};

	class MessageBus
	{
	public:
		void Subscribe(MessageType type, Subscriber* subscriber);

		void PostKeyPress(const KeyEvent& event);
		void PostMouseMove(const MouseEvent& event);
		void PostLevelComplete(std::string_view levelName);
		void PostEntityIntersect(const Entity* a, const Entity* b);
		void PostRestartLevel();

	private:
		std::unordered_map<MessageType, std::vector<Subscriber*>> m_Subscribers;
	};

}
