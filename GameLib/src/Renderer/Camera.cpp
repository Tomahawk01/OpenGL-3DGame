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
		, m_Right(vec3::Normalize(vec3::Cross(m_Direction, m_Up)))
		, m_Pitch{}
		, m_Yaw{ -std::numbers::pi_v<float> / 2.0f }
		, m_Fov(fov)
		, m_Width(width)
		, m_Height(height)
		, m_NearPlane(nearPlane)
		, m_FarPlane(farPlane)
	{
		m_Direction = CreateDirection(m_Pitch, m_Yaw);
		m_View = mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
		AddPitch(0.0f);
	}

	Camera::Camera(float width, float height, float depth)
		: m_View(mat4::LookAt(vec3{ 0.0f, 0.0f, 1.0f }, {}, {0.0f, 1.0f, 0.0f}))
		, m_Projection(mat4::Orthographic(width, height, depth))
		, m_Position(vec3{0.0f, 0.0f, 1.0f})
		, m_Direction(vec3{0.0f, 0.0f, -1.0f})
		, m_Up(vec3{0.0f, 1.0f, 0.0f})
		, m_Right(vec3::Normalize(vec3::Cross(m_Direction, m_Up)))
		, m_Pitch{}
		, m_Yaw{ -std::numbers::pi_v<float> / 2.0f }
		, m_Fov(0.0f)
		, m_Width(width)
		, m_Height(height)
		, m_NearPlane(0.0f)
		, m_FarPlane(depth)
	{}

	void Camera::Translate(const vec3& translation)
	{
		m_Position += translation;
		m_Direction = CreateDirection(m_Pitch, m_Yaw);
		m_View = mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
	}

	void Camera::AddYaw(float value)
	{
		m_Yaw += value;
		m_Direction = CreateDirection(m_Pitch, m_Yaw);

		const vec3 worldUp{ 0.0f, 1.0f, 0.0f };
		m_Right = vec3::Normalize(vec3::Cross(m_Direction, worldUp));
		m_Up = vec3::Normalize(vec3::Cross(m_Right, m_Direction));

		m_View = mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
	}

	void Camera::SetYaw(float yaw)
	{
		AddYaw(yaw - m_Yaw);
	}

	void Camera::AddPitch(float value)
	{
		m_Pitch += value;
		constexpr float pitchEpsilon = 0.0001f;
		m_Pitch = std::clamp(m_Pitch, (-std::numbers::pi_v<float> / 2.0f) + pitchEpsilon, (std::numbers::pi_v<float> / 2.0f) - pitchEpsilon);
		m_Direction = CreateDirection(m_Pitch, m_Yaw);

		const vec3 worldUp{ 0.0f, 1.0f, 0.0f };
		m_Right = vec3::Normalize(vec3::Cross(m_Direction, worldUp));
		m_Up = vec3::Normalize(vec3::Cross(m_Right, m_Direction));

		m_View = mat4::LookAt(m_Position, m_Position + m_Direction, m_Up);
	}

	void Camera::SetPitch(float pitch)
	{
		AddPitch(pitch - m_Pitch);
	}

	std::array<FrustumPlane, 6u> Camera::FrustumPlanes() const
	{
		std::array<FrustumPlane, 6u> planes{};

		const mat4 viewProj{ m_Projection * m_View };

		return {{
			{viewProj[3] - viewProj[2], viewProj[7] - viewProj[6], viewProj[11] - viewProj[10], viewProj[15] - viewProj[14]},
			{viewProj[3] + viewProj[2], viewProj[7] + viewProj[6], viewProj[11] + viewProj[10], viewProj[15] + viewProj[14]},
			{viewProj[3] + viewProj[0], viewProj[7] + viewProj[4], viewProj[11] + viewProj[8], viewProj[15] + viewProj[12]},
			{viewProj[3] - viewProj[0], viewProj[7] - viewProj[4], viewProj[11] - viewProj[8], viewProj[15] - viewProj[12]},
			{viewProj[3] + viewProj[1], viewProj[7] + viewProj[5], viewProj[11] + viewProj[9], viewProj[15] + viewProj[13]},
			{viewProj[3] - viewProj[1], viewProj[7] - viewProj[5], viewProj[11] - viewProj[9], viewProj[15] - viewProj[13]}
		}};
	}

	std::array<vec3, 8u> Camera::FrustumCorners() const
	{
		std::array<vec3, 8u> corners{};

		const float tanHalfFOV{ std::tan(m_Fov / 2.0f) };
		const float aspect{ m_Width / m_Height };

		const float nearHeight{ 2.0f * tanHalfFOV * m_NearPlane };
		const float nearWidth{ nearHeight * aspect };

		const float farHeight{ 2.0f * tanHalfFOV * m_FarPlane };
		const float farWidth{ farHeight * aspect };

		const vec3 forward{ vec3::Normalize(m_Direction) };
		const vec3 right{ vec3::Normalize(vec3::Cross(forward, m_Up)) };
		const vec3 up{ vec3::Normalize(vec3::Cross(right, forward)) };

		const vec3 nearCenter{ m_Position + m_Direction * m_NearPlane };
		corners[0] = nearCenter + up * (nearHeight / 2.0f) - right * (nearWidth / 2.0f);
		corners[1] = nearCenter + up * (nearHeight / 2.0f) + right * (nearWidth / 2.0f);
		corners[2] = nearCenter - up * (nearHeight / 2.0f) + right * (nearWidth / 2.0f);
		corners[3] = nearCenter - up * (nearHeight / 2.0f) - right * (nearWidth / 2.0f);

		const vec3 farCenter{ m_Position + m_Direction * m_FarPlane };
		corners[4] = farCenter + up * (farHeight / 2.0f) - right * (farWidth / 2.0f);
		corners[5] = farCenter + up * (farHeight / 2.0f) + right * (farWidth / 2.0f);
		corners[6] = farCenter - up * (farHeight / 2.0f) + right * (farWidth / 2.0f);
		corners[7] = farCenter - up * (farHeight / 2.0f) - right * (farWidth / 2.0f);

		return corners;
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

	vec3 Camera::Up() const
	{
		return m_Up;
	}

	vec3 Camera::Right() const
	{
		return m_Right;
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
