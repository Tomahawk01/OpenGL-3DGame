#include "Camera.h"

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
	{}

	void Camera::Translate(const vec3& translation)
	{
		m_Position += translation;
		m_Direction += translation;
		m_View = mat4::LookAt(m_Position, m_Direction, m_Up);
	}

	std::span<const float> Camera::GetView() const
	{
		return m_View.data();
	}

	std::span<const float> Camera::GetProjection() const
	{
		return m_Projection.data();
	}

}
