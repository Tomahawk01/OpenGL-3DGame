#pragma once

#include "Utilities/PassKey.h"
#include "Math/Vector3.h"
#include "Shape.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Renderer/DebugRenderer.h>

#include <memory>

namespace Game {

	class PhysicsSystem;

	class CharacterController : public JPH::CharacterContactListener
	{
	public:
		CharacterController(PhysicsSystem& ps, ::JPH::PhysicsSystem* physicsSystem, PassKey<PhysicsSystem>);

		vec3 GetPosition() const;

		void Move(const vec3 amount);

		void DebugDraw(::JPH::DebugRenderer* debugRenderer, PassKey<PhysicsSystem>) const;

		void Update(float delta, const ::JPH::BroadPhaseLayerFilter& broadPhaseLayerFilter, const ::JPH::ObjectLayerFilter& objectLayerFilter, PassKey<PhysicsSystem>);

		void SetLinearVelocity(const vec3& velocity);

		const Shape* GetShape();

		void OnContactAdded(
			const ::JPH::CharacterVirtual* inCharacter,
			const ::JPH::BodyID& inBodyID2,
			const ::JPH::SubShapeID& inSubShapeID2,
			::JPH::RVec3Arg inContactPosition,
			::JPH::Vec3Arg inContactNormal,
			::JPH::CharacterContactSettings& ioSettings) override;

	private:
		::JPH::Ref<::JPH::CharacterVirtual> m_Character;
		std::unique_ptr<::JPH::TempAllocator> m_TempAlloc;
		const Shape* m_Shape;
	};

}
