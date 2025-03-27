#include "Camera.h"

#include <numbers>

namespace {

	Game::vec3 CreateDirection(float pitch, float yaw)
	{
		return Game::vec3::Normalize({
			std::cos(pitch) * std::cos(yaw),
			std::sin(pitch),
			std::cos(pitch) * std::sin(yaw)
		});
	}

	Game::FrustumPlane ToPlane(const Game::vec3& p, const Game::vec3& n)
	{
		const auto normal = Game::vec3::Normalize(n);
		return { normal, Game::vec3::Dot(normal, p)};
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
		, m_Yaw{ -std::numbers::pi_v<float> / 2.0f }
		, m_Fov(fov)
		, m_Width(width)
		, m_Height(height)
		, m_NearPlane(nearPlane)
		, m_FarPlane(farPlane)
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

	std::array<Game::FrustumPlane, 6u> Camera::FrustumPlanes() const
	{
		const float halfVSide{ GetFarPlane() * std::tan(GetFOV() * 0.5f) };
		const float halfHSide{ halfVSide * (GetWidth() / GetHeight()) };
		const Game::vec3 frontMultFar{ GetDirection() * GetFarPlane() };
		const Game::vec3 up{ 0.0f, 1.0f, 0.0f };

		return {
			ToPlane(GetPosition() + GetNearPlane() * GetDirection(), GetDirection()),
			ToPlane(GetPosition() + frontMultFar, -GetDirection()),
			ToPlane(GetPosition(), Game::vec3::Cross(frontMultFar - RightVector() * halfHSide, up)),
			ToPlane(GetPosition(), Game::vec3::Cross(up, frontMultFar + RightVector() * halfHSide)),
			ToPlane(GetPosition(), Game::vec3::Cross(RightVector(), frontMultFar - up * halfVSide)),
			ToPlane(GetPosition(), Game::vec3::Cross(frontMultFar + up * halfVSide, RightVector()))
		};
	}

	std::span<const float> Camera::GetView() const
	{
		return m_View.data();
	}

	std::span<const float> Camera::GetProjection() const
	{
		return m_Projection.data();
	}

	vec3 Camera::GetPosition() const
	{
		return m_Position;
	}

	void Camera::SetPosition(const vec3& position)
	{
		m_Position = position;
		m_View = mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
	}

	vec3 Camera::GetDirection() const
	{
		return m_Direction;
	}

	float Camera::GetFOV() const
	{
		return m_Fov;
	}

	float Camera::GetWidth() const
	{
		return m_Width;
	}

	float Camera::GetHeight() const
	{
		return m_Height;
	}

	float Camera::GetNearPlane() const
	{
		return m_NearPlane;
	}

	float Camera::GetFarPlane() const
	{
		return m_FarPlane;
	}

}
