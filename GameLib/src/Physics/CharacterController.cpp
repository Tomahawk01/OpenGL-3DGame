#include "CharacterController.h"

#include "JoltUtils.h"
#include "PhysicsSystem.h"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

namespace Game {

	CharacterController::CharacterController(::JPH::PhysicsSystem* physicsSystem, PassKey<PhysicsSystem>)
		: m_Character{}
	{
		static constexpr float standingHeight = 5.0f;
		static constexpr float standingRadius = 2.5f;

		::JPH::Ref<::JPH::CharacterVirtualSettings> settings = new ::JPH::CharacterVirtualSettings();
		settings->mShape = ::JPH::RotatedTranslatedShapeSettings(
								::JPH::Vec3(0, 0.5f * standingHeight + standingRadius, 0),
								::JPH::Quat::sIdentity(),
								new ::JPH::CapsuleShape(0.5f * standingHeight, standingRadius))
								.Create()
								.Get();

		m_Character = new ::JPH::CharacterVirtual{ settings, ::JPH::RVec3::sZero(), ::JPH::Quat::sIdentity(), 0, physicsSystem };
	}

	vec3 CharacterController::GetPosition() const
	{
		return ToNative(m_Character->GetPosition());
	}

	void CharacterController::DebugDraw(::JPH::DebugRenderer* debugRenderer, PassKey<PhysicsSystem>) const
	{
		const auto transform = m_Character->GetCenterOfMassTransform();
		m_Character->GetShape()->Draw(debugRenderer, transform, ::JPH::Vec3(1, 1, 1), ::JPH::Color::sGreen, false, true);
	}

}
