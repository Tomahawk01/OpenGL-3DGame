#pragma once

#include "Math/Color.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"

#include "RigidBody.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Color.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace Game {

	vec3 ToNative(::JPH::RVec3Arg v);
	quat ToNative(::JPH::QuatArg& q);
	Color ToNative(::JPH::ColorArg c);

	::JPH::RVec3 ToJolt(const vec3& v);
	::JPH::Quat ToJolt(const quat& q);
	::JPH::ObjectLayer ToJolt(RigidBodyType type);
	::JPH::RMat44 ToJolt(const Transform& transform);
	::JPH::Color ToJolt(const Color& color);

}
