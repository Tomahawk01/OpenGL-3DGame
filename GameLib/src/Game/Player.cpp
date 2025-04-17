#include "Player.h"

#include "Utilities/Logger.h"

namespace Game {

	Player::Player(MessageBus& bus, Camera camera)
		: m_Camera(std::move(camera))
	{
		bus.Subscribe(MessageType::KEY_PRESS, this);
	}

	void Player::HandleKeyPress(const KeyEvent& event)
	{
		Logger::Trace("{}", event);
	}

	const Camera& Player::GetCamera() const
	{
		return m_Camera;
	}

	vec3 Player::GetPosition() const
	{
		return m_Camera.GetPosition();
	}

}
