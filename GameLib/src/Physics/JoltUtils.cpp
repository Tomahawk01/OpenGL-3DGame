#include "JoltUtils.h"

#include <concepts>

namespace Game {

	vec3 ToNative(::JPH::RVec3Arg v)
	{
		return { v.GetX(), v.GetY(), v.GetZ() };
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

}
