#pragma once

#include "Utilities/PassKey.h"
#include "Shape.h"

#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

namespace Game {

	class PhysicsSystem;

	class CylinderShape : public Shape
	{
	public:
		CylinderShape(float halfHeight, float radius, PassKey<PhysicsSystem>);

		float GetHalfHeight() const;
		float GetRadius() const;

		const ::JPH::ShapeSettings* GetNativeHandle() const override;

		std::string to_string() const;

	private:
		float m_HalfHeight;
		float m_Radius;
		::JPH::CylinderShapeSettings m_CylinderShapeSettings;
	};

}
