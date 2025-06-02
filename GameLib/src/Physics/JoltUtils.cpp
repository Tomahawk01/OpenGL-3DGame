#include "JoltUtils.h"

#include <concepts>
#include <ranges>

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

	::JPH::ObjectLayer ToJolt(RigidBodyType type)
	{
		return static_cast<::JPH::ObjectLayer>(std::to_underlying(type));
	}

	::JPH::RMat44 ToJolt(const Transform& transform)
	{
		const auto mat = static_cast<mat4>(transform);
		const auto chunks = mat.data() | std::views::chunk(4u);
		const auto cols = chunks | std::views::transform([](auto&& chunk) { return ::JPH::Float4(chunk[0], chunk[1], chunk[2], chunk[3]); });

		::JPH::Float4 floatArgs[4]{};
		std::ranges::copy(cols, floatArgs);

		return ::JPH::RMat44::sLoadFloat4x4(floatArgs);
	}

}
