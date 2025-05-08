#pragma once

#include "Utilities/PassKey.h"
#include "Shape.h"

#include <Jolt/Physics/Collision/Shape/SphereShape.h>

namespace Game {

	class PhysicsSystem;

	class SphereShape : public Shape
	{
	public:
		SphereShape(float radius, PassKey<PhysicsSystem>);

		float GetRadius() const;

		const ::JPH::ShapeSettings* GetNativeHandle() const override;

		std::string to_string() const;

	private:
		float m_Radius;
		::JPH::SphereShapeSettings m_SphereShapeSettings;
	};

}
