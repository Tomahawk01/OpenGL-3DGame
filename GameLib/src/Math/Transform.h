#pragma once

#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

#include <format>

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

	inline std::string to_string(const Transform& transform)
	{
		return std::format("pos:{}, scale:{}, rot:{}", transform.Position, transform.Scale, transform.Rotation);
	}

}
