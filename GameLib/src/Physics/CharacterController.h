#pragma once

#include "Utilities/PassKey.h"
#include "Math/Vector3.h"

#if !defined(JPH_DEBUG_RENDERER)
#define JPH_DEBUG_RENDERER
#endif
#include <Jolt/Jolt.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Renderer/DebugRenderer.h>

namespace Game {

	class PhysicsSystem;

	class CharacterController
	{
	public:
		CharacterController(::JPH::PhysicsSystem* physicsSystem, PassKey<PhysicsSystem>);

		vec3 GetPosition() const;

		void DebugDraw(::JPH::DebugRenderer* debugRenderer, PassKey<PhysicsSystem>) const;

	private:
		::JPH::Ref<::JPH::CharacterVirtual> m_Character;
	};

}
