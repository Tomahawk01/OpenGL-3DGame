#include "Player.h"

#include "Utilities/Logger.h"

#include <numbers>

namespace Game {

	Player::Player(Camera camera, CharacterController& controller)
		: m_Camera{ std::move(camera) }
		, m_KeyState{}
		, m_StartPosition{ m_Camera.GetPosition() }
		, m_Controller{ controller }
	{}

	void Player::Update()
	{
		vec3 walkDirection{ 0.0f, 0.0f, 0.0f };

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

		const float speed = 20.0f;
		const vec3 velocity = vec3::Normalize(walkDirection) * speed;
		m_Camera.Translate(velocity);
		m_Controller.SetLinearVelocity(velocity);
		m_Camera.SetPosition(m_Controller.GetPosition() + vec3(0.0f, 5.0f, 0.0f));
	}

	void Player::Restart()
	{
		m_Camera.SetYaw(-std::numbers::pi_v<float> / 2.0f);
		m_Camera.SetPitch(0.0f);
		m_Camera.SetPosition(m_StartPosition);
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

	void Player::HandleRestartLevel()
	{
		Restart();
	}

	Camera& Player::GetCamera()
	{
		return m_Camera;
	}

	vec3 Player::GetPosition() const
	{
		return m_Camera.GetPosition();
	}

}
