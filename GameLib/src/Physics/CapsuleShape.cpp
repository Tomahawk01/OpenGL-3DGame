#include "CapsuleShape.h"

#include <format>

namespace Game {

	CapsuleShape::CapsuleShape(float halfHeight, float radius, PassKey<PhysicsSystem> passKey)
		: Shape(ShapeType::CAPSULE, passKey)
		, m_HalfHeight{ halfHeight }
		, m_Radius{ radius }
		, m_CapsuleShape{ m_HalfHeight, m_Radius }
	{}

	float CapsuleShape::GetHalfHeight() const
	{
		return m_HalfHeight;
	}

	float CapsuleShape::GetRadius() const
	{
		return m_Radius;
	}

	const ::JPH::Shape* CapsuleShape::GetNativeHandle() const
	{
		return std::addressof(m_CapsuleShape);
	}

	std::string CapsuleShape::to_string() const
	{
		return std::format("CapsuleShape: {} {}", GetHalfHeight(), GetRadius());
	}

}
