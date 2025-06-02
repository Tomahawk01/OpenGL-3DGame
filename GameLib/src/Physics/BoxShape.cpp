#include "BoxShape.h"

#include "Utilities/Error.h"
#include "JoltUtils.h"

#include <format>

namespace Game {

	BoxShape::BoxShape(const vec3& dimensions, PassKey<PhysicsSystem> passKey)
		: Shape(passKey)
		, m_Dimensions{ dimensions }
		, m_BoxShape{ ToJolt(dimensions) }
	{}

	vec3 BoxShape::GetDimensions() const
	{
		return m_Dimensions;
	}

	const ::JPH::Shape* BoxShape::GetNativeHandle() const
	{
		return std::addressof(m_BoxShape);
	}

	std::string BoxShape::to_string() const
	{
		return std::format("BoxShape: {}", GetDimensions());
	}

}
