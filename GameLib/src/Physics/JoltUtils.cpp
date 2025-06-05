#include "JoltUtils.h"

#include <concepts>
#include <ranges>
#include <bit>

namespace Game {

	vec3 ToNative(::JPH::RVec3Arg v)
	{
		return { v.GetX(), v.GetY(), v.GetZ() };
	}

	quat ToNative(::JPH::QuatArg& q)
	{
		return { q.GetX(), q.GetY(), q.GetZ(), q.GetW() };
	}

	Color ToNative(::JPH::ColorArg c)
	{
		const auto toFloat = []<std::integral T>(T b) { return static_cast<float>(b) / static_cast<float>(std::numeric_limits<T>::max()); };
		return { toFloat(c.r), toFloat(c.g), toFloat(c.b) };
	}

	::JPH::RVec3 ToJolt(const vec3& v)
	{
		return { v.x, v.y, v.z };
	}

	::JPH::Quat ToJolt(const quat& q)
	{
		return { q.X, q.Y, q.Z, q.W };
	}

	::JPH::ObjectLayer ToJolt(RigidBodyType type)
	{
		return static_cast<::JPH::ObjectLayer>(std::to_underlying(type));
	}

	::JPH::RMat44 ToJolt(const Transform& transform)
	{
		const auto mat = static_cast<mat4>(transform);
		return std::bit_cast<::JPH::RMat44>(mat);
	}

}
