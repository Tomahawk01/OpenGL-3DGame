#include "Camera.h"

namespace {

	Game::vec3 CreateDirection(float pitch, float yaw)
	{
		return Game::vec3::Normalize({
			.x = std::cos(pitch) * std::cos(yaw),
			.y = std::sin(pitch),
			.z = std::cos(pitch) * std::sin(yaw)
		});
	}

}

namespace Game {

	Camera::Camera(const vec3& position,
				   const vec3& lookAt,
				   const vec3& up,
				   float fov,
				   float width, float height,
				   float nearPlane, float farPlane)
		: m_View(mat4::LookAt(position, lookAt, up))
		, m_Projection(mat4::Perspective(fov, width, height, nearPlane, farPlane))
		, m_Position(position)
		, m_Direction(lookAt)
		, m_Up(up)
		, m_Pitch{}
		, m_Yaw{}
	{}

	void Camera::Translate(const vec3& translation)
	{
		m_Position += translation;
		m_Direction = CreateDirection(m_Pitch, m_Yaw);
		m_View = mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
	}

	vec3 Camera::RightVector() const
	{
		return vec3::Normalize(vec3::Cross(m_Direction, m_Up));
	}

	std::span<const float> Camera::GetView() const
	{
		return m_View.data();
	}

	std::span<const float> Camera::GetProjection() const
	{
		return m_Projection.data();
	}

	vec3 Camera::GetDirection() const
	{
		return m_Direction;
	}

	void Camera::AddYaw(float value)
	{
		m_Yaw += value;
		m_Direction = CreateDirection(m_Pitch, m_Yaw);
		m_View = mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
	}

	void Camera::AddPitch(float value)
	{
		m_Pitch += value;
		m_Direction = CreateDirection(m_Pitch, m_Yaw);
		m_View = mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
	}

}
