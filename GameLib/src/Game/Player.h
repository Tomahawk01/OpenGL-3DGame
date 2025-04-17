#pragma once

#include "Math/Vector3.h"
#include "Messaging/Subscriber.h"
#include "Renderer/Camera.h"

namespace Game {

	class Player : public Subscriber
	{
	public:
		Player(MessageBus& bus, Camera camera);

		void HandleKeyPress(const KeyEvent& event) override;

		const Camera& GetCamera() const;
		vec3 GetPosition() const;

	private:
		Camera m_Camera;
	};

}
