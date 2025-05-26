#pragma once

#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/FrustumPlane.h"

#include <array>
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

		void AddYaw(float value);
		void SetYaw(float yaw);
		void AddPitch(float value);
		void SetPitch(float pitch);

		std::array<FrustumPlane, 6u> FrustumPlanes() const;
		std::array<vec3, 8u> FrustumCorners() const;

		std::span<const float> GetView() const;
		std::span<const float> GetProjection() const;
		vec3 GetPosition() const;
		void SetPosition(const vec3& position);
		vec3 GetDirection() const;
		vec3 Up() const;
		vec3 Right() const;
		float GetFOV() const;
		float GetWidth() const;
		float GetHeight() const;
		float GetNearPlane() const;
		float GetFarPlane() const;

	private:
		mat4 m_View;
		mat4 m_Projection;

		vec3 m_Position;
		vec3 m_Direction;
		vec3 m_Up;
		vec3 m_Right;

		float m_Pitch;
		float m_Yaw;

		float m_Fov;
		float m_Width;
		float m_Height;
		float m_NearPlane;
		float m_FarPlane;
	};

}
