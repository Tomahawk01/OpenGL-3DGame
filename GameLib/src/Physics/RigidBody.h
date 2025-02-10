#pragma once

#include "Utilities/PassKey.h"
#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Shape.h"

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyInterface.h>

namespace Game {

	class PhysicsSystem;

	enum class RigidBodyType
	{
		STATIC,
		DYNAMIC
	};

	class RigidBody
	{
	public:
		RigidBody(const Shape& shape, const vec3& position, RigidBodyType type, ::JPH::BodyInterface& bodyInterface, PassKey<PhysicsSystem>);

		RigidBody(const RigidBody&) = delete;
		RigidBody& operator=(const RigidBody&) = delete;
		RigidBody(RigidBody&&) = default;
		RigidBody& operator=(RigidBody&&) = default;

		vec3 GetPosition() const;
		quat GetRotation() const;
		RigidBodyType GetType() const;
		::JPH::Body* GetNativeHandle() const;

	private:
		::JPH::Body* m_Body;
		RigidBodyType m_Type;
		::JPH::BodyInterface* m_BodyInterface;
	};

}
