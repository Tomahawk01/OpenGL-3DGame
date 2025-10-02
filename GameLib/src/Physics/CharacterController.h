#pragma once

#include "Utilities/PassKey.h"
#include "Math/Vector3.h"
#include "TransformedShape.h"
#include "Shape.h"

#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include <memory>

namespace Game {

	class PhysicsSystem;

	class CharacterController
	{
	public:
		CharacterController(PhysicsSystem& ps, PassKey<PhysicsSystem>);

		TransformedShape GetTransformedShape() const;

		void Move(const vec3& amount);

		void DebugDraw(::JPH::DebugRenderer* debugRenderer, PassKey<PhysicsSystem>) const;

		void Update(float delta, PassKey<PhysicsSystem>);

		void SetLinearVelocity(const vec3& velocity);

	private:
		TransformedShape m_TransformedShape;
		vec3 m_LinearVelocity;
	};

}
