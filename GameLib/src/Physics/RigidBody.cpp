#include "RigidBody.h"

#include "Utilities/Exception.h"
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

	::JPH::ObjectLayer ToLayer(Game::RigidBodyType type)
	{
		switch (type)
		{
		case Game::RigidBodyType::STATIC: return 1u;
		case Game::RigidBodyType::DYNAMIC: return 0u;
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
	{
		const ::JPH::BodyCreationSettings bodySettings{ 
			shape.GetNativeHandle(),
			ToJolt(position),
			::JPH::Quat::sIdentity(),
			ToJoltType(m_Type),
			ToLayer(m_Type)
		};
		m_Body = bodyInterface.CreateBody(bodySettings);
		bodyInterface.AddBody(m_Body->GetID(), ToActivation(m_Type));
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
