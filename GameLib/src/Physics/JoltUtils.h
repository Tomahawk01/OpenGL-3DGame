#pragma once

#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>

namespace Game {

	vec3 ToNative(::JPH::RVec3Arg v);
	quat ToNative(::JPH::QuatArg& q);
	Color ToNative(::JPH::ColorArg c);

	::JPH::RVec3 ToJolt(const vec3& v);

}
