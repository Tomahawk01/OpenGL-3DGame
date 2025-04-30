#pragma once

#include "Math/Vector3.h"
#include "Messaging/Subscriber.h"
#include "Renderer/Camera.h"

#include <unordered_map>

namespace Game {

	class Player : public Subscriber
	{
	public:
		Player(MessageBus& bus, Camera camera);

		void Update();

		void HandleKeyPress(const KeyEvent& event) override;
		void HandleMouseMove(const MouseEvent& event) override;

		const Camera& GetCamera() const;
		vec3 GetPosition() const;

	private:
		Camera m_Camera;
		std::unordered_map<Key, bool> m_KeyState;
	};

}
