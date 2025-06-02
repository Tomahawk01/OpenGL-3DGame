#pragma once

#include "Utilities/PassKey.h"
#include "Math/Vector3.h"
#include "Shape.h"

#include <Jolt/Physics/Collision/Shape/BoxShape.h>

namespace Game {

	class PhysicsSystem;

	class BoxShape : public Shape
	{
	public:
		BoxShape(const vec3& dimensions, PassKey<PhysicsSystem>);

		vec3 GetDimensions() const;

		const ::JPH::Shape* GetNativeHandle() const override;

		std::string to_string() const;

	private:
		vec3 m_Dimensions;
		::JPH::BoxShape m_BoxShape;
	};

}
