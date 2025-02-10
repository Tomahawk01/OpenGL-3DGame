#include "RigidBody.h"

#include "Utilities/Exception.h"
#include "PhysicsSystem.h"
#include "JoltUtils.h"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/MotionType.h>

namespace {

	::JPH::EMotionType ToJoltType(Game::RigidBodyType type)
	{
		switch (type)
		{
		case Game::RigidBodyType::STATIC: return ::JPH::EMotionType::Static;
		case Game::RigidBodyType::DYNAMIC: return ::JPH::EMotionType::Dynamic;
		}

		throw Game::Exception("Unknown type");
	}

	Game::PhysicsLayer ToLayer(Game::RigidBodyType type)
	{
		switch (type)
		{
		case Game::RigidBodyType::STATIC: return Game::PhysicsLayer::NON_MOVING;
		case Game::RigidBodyType::DYNAMIC: return Game::PhysicsLayer::MOVING;
		}

		throw Game::Exception("Unknown type");
	}

	::JPH::EActivation ToActivation(Game::RigidBodyType type)
	{
		switch (type)
		{
		case Game::RigidBodyType::STATIC: return ::JPH::EActivation::DontActivate;
		case Game::RigidBodyType::DYNAMIC: return ::JPH::EActivation::Activate;
		}

		throw Game::Exception("Unknown type");
	}

}

namespace Game {

	RigidBody::RigidBody(const Shape& shape, const vec3& position, RigidBodyType type, ::JPH::BodyInterface& bodyInterface, PassKey<PhysicsSystem>)
		: m_Body{}
		, m_Type{ type }
		, m_BodyInterface{ std::addressof(bodyInterface) }
	{
		const ::JPH::BodyCreationSettings bodySettings{
			shape.GetNativeHandle(),
			ToJolt(position),
			::JPH::Quat::sIdentity(),
			ToJoltType(m_Type),
			static_cast<uint16_t>(std::to_underlying(ToLayer(m_Type)))
		};

		m_Body = bodyInterface.CreateBody(bodySettings);
		m_BodyInterface->AddBody(m_Body->GetID(), ToActivation(m_Type));
	}

	void RigidBody::SetLinearVelocity(const vec3& impulse) const
	{
		m_BodyInterface->SetLinearVelocity(m_Body->GetID(), ToJolt(impulse));
	}

	vec3 RigidBody::GetPosition() const
	{
		const auto position = m_BodyInterface->GetPosition(m_Body->GetID());
		return ToNative(position);
	}

	quat RigidBody::GetRotation() const
	{
		const auto rotation = m_BodyInterface->GetRotation(m_Body->GetID());
		return ToNative(rotation);
	}

	RigidBodyType RigidBody::GetType() const
	{
		return m_Type;
	}

	::JPH::Body* RigidBody::GetNativeHandle() const
	{
		return m_Body;
	}

}
