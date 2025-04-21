#include "Player.h"

#include "Utilities/Logger.h"

namespace Game {

	Player::Player(MessageBus& bus, Camera camera)
		: m_Camera{ std::move(camera) }
		, m_KeyState{}
	{
		bus.Subscribe(MessageType::KEY_PRESS, this);
		bus.Subscribe(MessageType::MOUSE_MOVE, this);
	}

	void Player::Update()
	{
		vec3 walkDirection{ 0.0f, 0.0f, 0.0f };
		walkDirection.y = 0.0f;

		if (m_KeyState[Key::W])
		{
			walkDirection += m_Camera.GetDirection();
		}
		if (m_KeyState[Key::A])
		{
			walkDirection -= m_Camera.Right();
		}
		if (m_KeyState[Key::S])
		{
			walkDirection -= m_Camera.GetDirection();
		}
		if (m_KeyState[Key::D])
		{
			walkDirection += m_Camera.Right();
		}

		walkDirection.y = 0.0f;

		const float speed = 0.5f;
		const vec3 velocity = vec3::Normalize(walkDirection) * speed;
		m_Camera.Translate(velocity);
	}

	void Player::HandleKeyPress(const KeyEvent& event)
	{
		m_KeyState[event.GetKey()] = event.GetState() == KeyState::DOWN;
	}

	void Player::HandleMouseMove(const MouseEvent& event)
	{
		static constexpr float sensitivity{ 0.002f };
		const float deltaX = event.GetDeltaX() * sensitivity;
		const float deltaY = event.GetDeltaY() * sensitivity;

		m_Camera.AddYaw(deltaX);
		m_Camera.AddPitch(-deltaY);
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
