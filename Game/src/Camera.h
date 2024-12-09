#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"

#include <span>

namespace Game {

	class Camera
	{
	public:
		Camera(const vec3& position,
			   const vec3& lookAt, 
			   const vec3& up,
			   float fov,
			   float width, float height,
			   float nearPlane, float farPlane);

		void Translate(const vec3& translation);
		vec3 RightVector() const;

		std::span<const float> GetView() const;
		std::span<const float> GetProjection() const;
		vec3 GetPosition() const;
		vec3 GetDirection() const;

		void AddYaw(float value);
		void AddPitch(float value);

	private:
		mat4 m_View;
		mat4 m_Projection;

		vec3 m_Position;
		vec3 m_Direction;
		vec3 m_Up;

		float m_Pitch;
		float m_Yaw;
	};

}
