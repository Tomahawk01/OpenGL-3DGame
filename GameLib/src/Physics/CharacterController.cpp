#include "CharacterController.h"

#include "Utilities/Logger.h"

#include "JoltUtils.h"
#include "PhysicsSystem.h"
#include "CapsuleShape.h"

namespace {
	constexpr float standingHeight = 3.0f;
	constexpr float standingRadius = 1.25f;
}

namespace Game {

	CharacterController::CharacterController(PhysicsSystem& ps, PassKey<PhysicsSystem>)
		: m_TransformedShape{ ps.CreateShape<CapsuleShape>(0.5f * standingHeight, standingRadius), {{}, {1.0f}, {}} }
		, m_LinearVelocity{}
	{}

	TransformedShape CharacterController::GetTransformedShape() const
	{
		return m_TransformedShape;
	}

	void CharacterController::Move(const vec3& amount)
	{
		m_TransformedShape.Translate(amount);
	}

	void CharacterController::DebugDraw(::JPH::DebugRenderer* debugRenderer, PassKey<PhysicsSystem>) const
	{
		m_TransformedShape.GetShape()->GetNativeHandle()->Draw(debugRenderer, ToJolt(m_TransformedShape.GetTransform()), ::JPH::Vec3(1, 1, 1), ::JPH::Color::sGreen, false, true);
	}

	void CharacterController::Update(float delta, PassKey<PhysicsSystem>)
	{
		m_TransformedShape.Translate(m_LinearVelocity * delta);
	}

	void CharacterController::SetLinearVelocity(const vec3& velocity)
	{
		m_LinearVelocity = velocity;
	}

}
