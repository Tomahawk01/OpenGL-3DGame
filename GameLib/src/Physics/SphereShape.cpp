#include "SphereShape.h"

#include <format>

namespace Game {

	SphereShape::SphereShape(float radius, PassKey<PhysicsSystem> passKey)
		: Shape(passKey)
		, m_Radius{ radius }
		, m_SphereShapeSettings{ m_Radius }
	{
		m_SphereShapeSettings.SetEmbedded();
	}

	float SphereShape::GetRadius() const
	{
		return m_Radius;
	}

	const ::JPH::ShapeSettings* SphereShape::GetNativeHandle() const
	{
		return std::addressof(m_SphereShapeSettings);
	}

	std::string SphereShape::to_string() const
	{
		return std::format("SphereShape: {}", GetRadius());
	}

}
