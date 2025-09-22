#pragma once

#include "Math/Vector3.h"
#include "Messaging/Subscriber.h"
#include "Messaging/AutoSubscribe.h"
#include "Renderer/Camera.h"
#include "Physics/CharacterController.h"

#include <unordered_map>

namespace Game {

	class Player
	{
	public:
		Player(Camera camera, CharacterController& controller);

		void Update();
		void Restart();

		// NOTE: These are sent from LevelRoutine instead of MessageBus
		void HandleKeyPress(const KeyEvent& event);
		void HandleMouseMove(const MouseEvent& event);
		void HandleRestartLevel();

		Camera& GetCamera();
		vec3 GetPosition() const;

		auto& GetController(this auto&& self)
		{
			return self.m_Controller;
		}

	private:
		Camera m_Camera;
		std::unordered_map<Key, bool> m_KeyState;
		vec3 m_StartPosition;
		CharacterController& m_Controller;
	};

}
