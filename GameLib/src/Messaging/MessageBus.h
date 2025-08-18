#pragma once

#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"

#include "Game/GameState.h"

#include <string_view>
#include <unordered_map>
#include <vector>

namespace Game {

	class Subscriber;
	class Entity;
	class Camera;
	struct Scene;

	enum class MessageType
	{
		KEY_PRESS,
		MOUSE_MOVE,
		LEVEL_COMPLETE,
		ENTITY_INTERSECT,
		RESTART_LEVEL,
		QUIT,
		STATE_CHANGE,
		NEW_CAMERA,
		NEW_SCENE,
	};

	class MessageBus
	{
	public:
		MessageBus() = default;
		~MessageBus() = default;

		MessageBus(const MessageBus&) = delete;
		MessageBus& operator=(const MessageBus&) = delete;
		MessageBus(MessageBus&&) = default;
		MessageBus& operator=(MessageBus&&) = default;

		void Subscribe(MessageType type, Subscriber* subscriber);
		void Unsubscribe(MessageType type, Subscriber* subscriber);

		void PostKeyPress(const KeyEvent& event);
		void PostMouseMove(const MouseEvent& event);
		void PostLevelComplete(std::string_view levelName);
		void PostEntityIntersect(const Entity* a, const Entity* b);
		void PostRestartLevel();
		void PostQuit();
		void PostStateChange(GameState state);
		void PostNewCamera(Camera* camera);
		void PostNewScene(Scene* scene);

	private:
		std::unordered_map<MessageType, std::vector<Subscriber*>> m_Subscribers;
	};

}
