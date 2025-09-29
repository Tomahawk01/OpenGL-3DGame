#include "CharacterController.h"

#include "Utilities/Logger.h"

#include "JoltUtils.h"
#include "PhysicsSystem.h"
#include "CapsuleShape.h"

#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

namespace {
	constexpr float standingHeight = 3.0f;
	constexpr float standingRadius = 1.25f;
}

namespace Game {

	CharacterController::CharacterController(PhysicsSystem& ps, ::JPH::PhysicsSystem* physicsSystem, PassKey<PhysicsSystem>)
		: m_Character{}
		, m_TempAlloc{ std::make_unique<::JPH::TempAllocatorImpl>(4 * 1024 * 1024) }
		, m_Shape{ ps.CreateShape<CapsuleShape>(0.5f * standingHeight, standingRadius) }
	{
		::JPH::Ref<::JPH::CharacterVirtualSettings> settings = new ::JPH::CharacterVirtualSettings();
		settings->mShape = ::JPH::RotatedTranslatedShapeSettings(
								::JPH::Vec3(0, 0.5f * standingHeight + standingRadius, 0),
								::JPH::Quat::sIdentity(),
								m_Shape->GetNativeHandle())
								.Create()
								.Get();
		settings->mInnerBodyLayer = ToJolt(RigidBodyType::DYNAMIC);

		m_Character = new ::JPH::CharacterVirtual{ settings, ::JPH::RVec3::sZero(), ::JPH::Quat::sIdentity(), 0, physicsSystem };
		m_Character->SetListener(this);
	}

	vec3 CharacterController::GetPosition() const
	{
		return ToNative(m_Character->GetPosition());
	}

	void CharacterController::Move(const vec3 amount)
	{
		m_Character->SetPosition(ToJolt(GetPosition() + amount));
	}

	void CharacterController::DebugDraw(::JPH::DebugRenderer* debugRenderer, PassKey<PhysicsSystem>) const
	{
		const auto transform = m_Character->GetCenterOfMassTransform();
		m_Character->GetShape()->Draw(debugRenderer, transform, ::JPH::Vec3(1, 1, 1), ::JPH::Color::sGreen, false, true);
	}

	void CharacterController::Update(float delta, const ::JPH::BroadPhaseLayerFilter& broadPhaseLayerFilter, const ::JPH::ObjectLayerFilter& objectLayerFilter, PassKey<PhysicsSystem>)
	{
		m_Character->Update(delta, ::JPH::Vec3{ 0.0f, -9.8f, 0.0f }, broadPhaseLayerFilter, objectLayerFilter, {}, {}, *m_TempAlloc);
	}

	void CharacterController::SetLinearVelocity(const vec3& velocity)
	{
		m_Character->SetLinearVelocity(ToJolt(velocity));
	}

	const Shape* CharacterController::GetShape()
	{
		return m_Shape;
	}

	void CharacterController::OnContactAdded(
		const ::JPH::CharacterVirtual* inCharacter,
		const ::JPH::BodyID& inBodyID2,
		const ::JPH::SubShapeID& inSubShapeID2,
		::JPH::RVec3Arg inContactPosition,
		::JPH::Vec3Arg inContactNormal,
		::JPH::CharacterContactSettings& ioSettings)
	{
		Logger::Trace("Contact {}", inBodyID2.GetIndex());
	}

}
