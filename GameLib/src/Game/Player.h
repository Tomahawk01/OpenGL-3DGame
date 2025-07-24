#pragma once

#include "Math/Vector3.h"
#include "Messaging/Subscriber.h"
#include "Messaging/AutoSubscribe.h"
#include "Renderer/Camera.h"

#include <unordered_map>

namespace Game {

	class Player : public Subscriber
	{
	public:
		Player(MessageBus& bus, Camera camera);

		void Update();
		void Restart();

		void HandleKeyPress(const KeyEvent& event) override;
		void HandleMouseMove(const MouseEvent& event) override;
		void HandleRestartLevel() override;

		const Camera& GetCamera() const;
		vec3 GetPosition() const;

	private:
		Camera m_Camera;
		std::unordered_map<Key, bool> m_KeyState;
		vec3 m_StartPosition;
		AutoSubscribe m_AutoSub;
	};

}
