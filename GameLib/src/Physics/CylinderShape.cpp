#if 0
#include "CylinderShape.h"

#include <format>

namespace Game {

	CylinderShape::CylinderShape(float halfHeight, float radius, PassKey<PhysicsSystem> passKey)
		: Shape(passKey)
		, m_HalfHeight{ halfHeight }
		, m_Radius{ radius }
		, m_CylinderShapeSettings{ halfHeight, radius }
	{
		m_CylinderShapeSettings.SetEmbedded();
	}

	float CylinderShape::GetHalfHeight() const
	{
		return m_HalfHeight;
	}

	float CylinderShape::GetRadius() const
	{
		return m_Radius;
	}

	const ::JPH::ShapeSettings* CylinderShape::GetNativeHandle() const
	{
		return std::addressof(m_CylinderShapeSettings);
	}

	std::string CylinderShape::to_string() const
	{
		return std::format("CylinderShape: {} {}", GetHalfHeight(), GetRadius());
	}

}
#endif