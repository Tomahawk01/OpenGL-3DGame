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
	{}

	std::span<const float> Camera::GetView() const
	{
		return m_View.data();
	}

	std::span<const float> Camera::GetProjection() const
	{
		return m_Projection.data();
	}

}
