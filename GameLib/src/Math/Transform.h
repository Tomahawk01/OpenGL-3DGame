#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

namespace Game {

	class Transform
	{
	public:
		constexpr Transform(const vec3& position, const vec3& scale, const quat& rotation)
			: Position(position), Scale(scale), Rotation(rotation)
		{}

		constexpr operator mat4() const
		{
			return mat4{ Position } * mat4{ Rotation } * mat4{ Scale, mat4::Scale{} };
		}

		vec3 Position;
		vec3 Scale;
		quat Rotation;
	};

}
